#include "Thread.h"

#include <pthread.h>
#include <time.h>
#include <sched.h>

#include <cstdio>
#include <cassert>
#include <map>

#include "MutexLock.h"

// ::pthread_setname_np() Is available since GLIBC 2.11
// ubuntu 9.04 GLIBC is old and cannot be used
#define THREAD_USE_SETNAME_NP 0

namespace LightIPC {

static std::map<unsigned long, Thread*> threadMap;
static Mutex threadMapMutex;

static const unsigned long BILLION = 1000000000;

unsigned long Thread::CurrentThreadId()
{
	return ::pthread_self();
}

void Thread::Yield()
{
	::sched_yield();
	struct timespec yield_sleep = {0,100}; // nano
	::nanosleep(&yield_sleep, NULL);
}

void Thread::Sleep(unsigned int sec)
{
	::sleep(sec);
}

static void NanoSleepFunc(unsigned long sec, unsigned long nsec)
{
	struct timespec req_sleep = {sec,nsec};
	::nanosleep(&req_sleep, NULL);
}

void Thread::MilliSleep(unsigned int millisec)
{
	unsigned long time = millisec*1000000;
	unsigned long sec  = time/BILLION;
	unsigned long nsec = time%BILLION;
	NanoSleepFunc(sec, nsec);
}

void Thread::MicroSleep(unsigned int microsec)
{
	unsigned long time = microsec*1000;
	unsigned long sec  = time/BILLION;
	unsigned long nsec = time%BILLION;
	NanoSleepFunc(sec, nsec);
}

void Thread::NanoSleep(unsigned int _nsec)
{
	unsigned long time = _nsec;
	unsigned long sec  = time/BILLION;
	unsigned long nsec = time%BILLION;
	NanoSleepFunc(sec, nsec);
}

Thread *Thread::CurrentThread()
{
	MutexLock l(&threadMapMutex);
	std::map<unsigned long, Thread*>::iterator it = threadMap.find(Thread::CurrentThreadId());
	if (it != threadMap.end()) {
		return it->second;
	}
	return NULL;
}

	
Thread::Thread()
	: m_thread()
	, m_runnable(NULL)
	, m_parameter(NULL)
	, m_name()
	, m_threadId(0)
	, m_isActive(false)
{
}

Thread::Thread(IRunnable *runnable, void *param)
	: m_thread()
	, m_runnable(runnable)
	, m_parameter(param)
	, m_name()
	, m_threadId(0)
	, m_isActive(false)
{
}

Thread::~Thread()
{
}

void Thread::SetRunner(IRunnable *runnable, void *param)
{
	m_runnable = runnable;
	m_parameter = param;
}

void *Thread::Parameter()
{
	return m_parameter;
}

void Thread::SetName(const std::string &name)
{
	// max length 16 char(include '\0')
	m_name = name.substr(0,15);
#if THREAD_USE_SETNAME_NP 
	if (::pthread_equal(m_thread, Thread::CurrentThreadId())) {
		::pthread_setname_np(m_thread, m_name.c_str());
	}
#endif
}

std::string Thread::Name()
{
	return m_name;
}

unsigned long Thread::ThreadId()
{
	return m_threadId;
}

bool Thread::IsActive()
{
	return m_isActive;
}

struct WrapperParam
{
	Thread *self;
	pthread_t *tid;
	unsigned long *threadId;
	bool *isActive;
};

// pthread_cleanup_push() Pass to cleanup handler
// pthread_cleanup_push() Is a macro that supports only C functions
extern "C" {
void thread_cleanup_handler(void *param)
{
	WrapperParam *p = static_cast<WrapperParam *>(param);
	Thread *self = p->self;
	// Thread Does Cancel() with Lock() on Mutex
	// Unlock() Because it may not be deadlocked
	// Release resources etc. is handled by Cleanup()
	self->Cleanup();

	// Deleted from threadMap because the thread has finished
	{
		MutexLock l(&threadMapMutex);
		int eraseCount = threadMap.erase(*p->threadId);
		if (eraseCount != 1) {
			std::printf("thread cleanup error [%s:%d]\n", "erased any threads", eraseCount);
			assert(eraseCount == 1);
		}
		*p->threadId = 0;
		*p->isActive = false;
		delete p;
	}
}
}

void *wrapperFunction(void *param)
{
	WrapperParam *p = static_cast<WrapperParam *>(param);
	Thread *self = p->self;
#if THREAD_USE_SETNAME_NP 
	pthread_t *tid = p->tid;
	::pthread_setname_np(*tid, self->Name().c_str());
#endif
	unsigned long *threadId = p->threadId;
	bool *isActive = p->isActive;
	
	// Adds to threadMap as it starts a thread
	unsigned long currentTid = Thread::CurrentThreadId();
	{
		MutexLock l(&threadMapMutex);
		threadMap.insert(std::make_pair(currentTid, self));
		*threadId = currentTid;
		*isActive = true;
	}

	// Cancel() Sometimes it is not executed after self->execute()
	// pthread_cleanup_push() To describe the cleanup process
	pthread_cleanup_push(thread_cleanup_handler, p);

	// Thread start
	self->Execute();

	// param 0 Does nothing with (unregister function only)
	// 0 Otherwise, execute the handler and cancel function registration
	pthread_cleanup_pop(1);

	return NULL;
}

void Thread::Execute()
{
	if (m_runnable) {
		m_runnable->Run();
	}
}

void Thread::Cleanup()
{
	if (m_runnable) {
		m_runnable->Cleanup();
	}
}

void Thread::Start()
{
	if (m_isActive) {
		return;
	}
	m_isActive = true;

	WrapperParam *param = new WrapperParam();
	param->self = this;
	param->tid = &m_thread;
	param->threadId = &m_threadId;
	param->isActive = &m_isActive;
	::pthread_create(&m_thread, NULL, wrapperFunction, param);
}

void Thread::Join()
{
	void *thread_return;
	::pthread_join(m_thread, &thread_return);
}

void Thread::Cancel()
{
	::pthread_cancel(m_thread);
}

}
