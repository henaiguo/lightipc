#include "MessageQueue.h"

#include <fcntl.h>
#include <time.h>
#include <signal.h>

#include <cstdio>
#include <cerrno>
#include <cassert>
#include <cstring>

namespace LightIPC {

static const unsigned int PRIORITY = 10; //  POSIX:0 <= 31, LINUX:0 <= sysconf(_SC_MQ_PRIO_MAX)[=32768]
static const long BILLION = 1000000000;
static const long MILLION = 1000000;

struct timespec timeAdd(struct timespec t1, struct timespec t2)
{
	long sec = t2.tv_sec + t1.tv_sec;
	long nsec = t2.tv_nsec + t1.tv_nsec;
	if (nsec >= BILLION) {
		nsec -= BILLION;
		sec++;
	}
	struct timespec add = { sec, nsec };
	return add;
}


/*
Message Queue can be mounted on a file system
Linux In, the message queue is created in the virtual file system

# mkdir /path/to/file
# sudo mount -t mqueue none /path/to/file
# ls /path/to/file
# cat /path/to/file
*/
Result MessageQueue::Exist(const std::string &name)
{
	if (name.size() == 0) {
		return Result::CreateError("message queue invalid name [%s]\n","empty name");
	}

	if (name[0] != '/') {
		return Result::CreateError("message queue invalid name [%s]\n","it does not begin with '/'");
	}

	const char *named = name.c_str();

	// open POSIX message queue
	mqd_t mq = ::mq_open(named, O_RDONLY);
	if (mq == -1) {
		return Result::CreateError("message queue open error [%s]\n", std::strerror(errno));
	}
	::mq_close(mq);

	return Result::CreateSuccess();
}

MessageQueue::MessageQueue(const std::string &name)
	: m_name(name)
	, m_isOwner(false)
	, m_messageQueue()
	, m_attribute()
	, m_notification(NULL)
{
	Init(0,0);
}

MessageQueue::MessageQueue(const std::string &name, long maxMessageCount, long maxMessageSize)
	: m_name(name)
	, m_isOwner(true)
	, m_messageQueue()
	, m_attribute()
	, m_notification(NULL)
{
	Init(maxMessageCount, maxMessageSize);
}

void MessageQueue::Init(long maxMessageCount, long maxMessageSize)
{
	if (m_name.size() == 0) {
		std::fprintf(stderr, "message queue creation error [%s]\n","empty name");
		return;
	}

	if (m_name[0] != '/') {
		std::fprintf(stderr, "message queue creation error [%s]\n","it does not begin with '/'");
		return;
	}

	const char *named = m_name.c_str();

	// remove old message queue
	if (m_isOwner) {
		::mq_unlink(named);
	}

	// create message queue
	if (m_isOwner) {
		mq_attr attr;
		attr.mq_flags   = 0; // or O_NONBLOCK
		attr.mq_maxmsg  = (maxMessageCount > 0 ? maxMessageCount : 10);
		attr.mq_msgsize = (maxMessageSize  > 0 ? maxMessageSize : 8192);
		attr.mq_curmsgs = 0;
		m_messageQueue = ::mq_open(named, O_CREAT|O_RDWR|O_EXCL, S_IRUSR|S_IWUSR, &attr);
	} else {
		m_messageQueue = ::mq_open(named, O_RDWR);
	}

	if (m_messageQueue == -1) {
		std::fprintf(stderr, "message queue creation error [%s]\n", std::strerror(errno));
		return;
	}
	
	if (::mq_getattr(m_messageQueue, &m_attribute) == -1) {
		m_attribute.mq_flags   = -1;
		m_attribute.mq_maxmsg  = -1;
		m_attribute.mq_msgsize = -1;
		m_attribute.mq_curmsgs = -1;
		std::fprintf(stderr, "message queue attribute error [%s]\n", std::strerror(errno));
		return;
	}
}

MessageQueue::~MessageQueue()
{
	if (m_messageQueue != -1) {
		::mq_close(m_messageQueue);
		if (m_isOwner) {
			const char *named = m_name.c_str();
			::mq_unlink(named);
		}
	}
}

const std::string &MessageQueue::Name() const
{
	return m_name;
}


long MessageQueue::MaxMessageCount()
{
	if (m_messageQueue == -1) {
		return -1;
	}

	return m_attribute.mq_maxmsg;
}

long MessageQueue::MaxMessageSize()
{
	if (m_messageQueue == -1) {
		return -1;
	}

	return m_attribute.mq_msgsize;
}

long MessageQueue::CurrentMessageCount()
{
	if (m_messageQueue == -1) {
		return -1;
	}

	if (::mq_getattr(m_messageQueue, &m_attribute) == -1) {
		return -1;
	}

	return m_attribute.mq_curmsgs;
}

void MessageQueue::Clear()
{
	// m_messageQueue Existence check, current queue number, maximum message size
	// Are checked at the same time
	if (CurrentMessageCount() <= 0) {
		return;
	}

	size_t buf_len = static_cast<size_t>(MaxMessageSize());
	char buf[buf_len]; // C99 OK if above

	struct timespec timeout = {0,0};
	ssize_t receiveSize = 0;
	do {
		receiveSize = ::mq_timedreceive(m_messageQueue, buf, buf_len, NULL, &timeout);
		if (receiveSize == -1) {
			break;
		}
	} while (true);
}

Result MessageQueue::Send(const ByteBuffer &message)
{
	return TimedSend(message, 0UL);
}

Result MessageQueue::TimedSend(const ByteBuffer &message, unsigned long millisec)
{
	if (m_messageQueue == -1) {
		return Result::CreateError("message queue send error [%s]","queue not found");
	}

	int ret = 0;
	if (millisec == 0) {
		ret = ::mq_send(m_messageQueue, message.Data().c_str(), message.Size(), PRIORITY);
	} else {
		struct timespec now;
		::clock_gettime(CLOCK_REALTIME, &now);
		long time = millisec*MILLION;
		long sec  = time/BILLION;
		long nsec = time%BILLION;
		struct timespec usertime = {sec, nsec};
		struct timespec timeout = timeAdd(now, usertime);
		ret = ::mq_timedsend(m_messageQueue, message.Data().c_str(), message.Size(), PRIORITY, &timeout);
	}

	if (ret == -1) {
		return Result::CreateError("message queue send error [%s]",std::strerror(errno));
	}

	return Result::CreateSuccess();
}

Result MessageQueue::Receive(ByteBuffer &outMessage)
{
	return TimedReceive(outMessage, 0UL);
}

Result MessageQueue::TimedReceive(ByteBuffer &outMessage, unsigned long millisec)
{
	if (m_messageQueue == -1) {
		return Result::CreateError("message queue receive error [%s]","queue not found");
	}

	long len = MaxMessageSize();
	if (len < 0) {
		return Result::CreateError("message queue receive error [%s:%ld]","invalid message size", len);
	}

	ssize_t receiveSize = 0;
	size_t buf_len = static_cast<size_t>(len);
	char buf[buf_len]; // C99 OK if above
	unsigned int priority = 0;
	if (millisec == 0) {
		receiveSize = ::mq_receive(m_messageQueue, buf, buf_len, &priority);
	} else {
		struct timespec now;
		::clock_gettime(CLOCK_REALTIME, &now);
		long time = millisec*MILLION;
		long sec  = time/BILLION;
		long nsec = time%BILLION;
		struct timespec usertime = {sec, nsec};
		struct timespec timeout = timeAdd(now, usertime);
		receiveSize = ::mq_timedreceive(m_messageQueue, buf, buf_len, &priority, &timeout);
	}

	if (receiveSize == -1) {
		return Result::CreateError("message queue receive error [%s]",std::strerror(errno));
	}

	outMessage = ByteBuffer(buf, static_cast<size_t>(receiveSize));

	return Result::CreateSuccess();
}

Result MessageQueue::Receive(std::vector<ByteBuffer> &outMessages)
{
	outMessages.clear();
	long currentCount = CurrentMessageCount();
	if (currentCount < 0) {
		return Result::CreateError("message queue receive error [%s:%ld]","invalid message count", currentCount);
	}

	if (currentCount == 0) {
		return Result::CreateSuccess();
	}

	ssize_t receiveSize = 0;
	size_t buf_len = static_cast<size_t>(MaxMessageSize());
	char buf[buf_len]; // C99 OK if above
	Result result;
	do {
		struct timespec timeout = {0,0};
		receiveSize = ::mq_timedreceive(m_messageQueue, buf, buf_len, NULL, &timeout);
		if (receiveSize == -1) {
			if (errno == ETIMEDOUT) {
				result = Result::CreateSuccess();
			} else {
				outMessages.clear();
				result = Result::CreateError("message queue receive error [%s]",std::strerror(errno));
			}
			break;
		}

		ByteBuffer bb(buf, static_cast<size_t>(receiveSize));
		outMessages.push_back(bb);
	} while (true);

	return result;
}

static void signalNotifyFunction(sigval sv)
{
	MessageQueue *mq = static_cast<MessageQueue *>(sv.sival_ptr);

	// INotifyMessage Re-register
	// mq_notify() Is only when adding a new message to the empty queue
	// It is called, but once signaled, the setting will be canceled so
	// Register again and then process the message
	INotifyMessage *notification = mq->NotifyMessage();
	if (notification) {
		mq->SetNotifyMessage(notification);
		notification->FirstMessageArrived(*mq);
	}
}

void MessageQueue::SetNotifyMessage(INotifyMessage *notification)
{
	m_notification = notification;
	if (m_notification == NULL) {
		::mq_notify(m_messageQueue, NULL);
		return;
	}

	m_signalEvent.sigev_notify = SIGEV_THREAD;
	m_signalEvent.sigev_notify_function = signalNotifyFunction;
	m_signalEvent.sigev_notify_attributes = NULL;
	m_signalEvent.sigev_value.sival_ptr = this;
	if (::mq_notify(m_messageQueue, &m_signalEvent) == -1) {
		return;
	}
}

INotifyMessage *MessageQueue::NotifyMessage()
{
	return m_notification;
}

}
