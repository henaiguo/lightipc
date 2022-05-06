#include "UnixDomainSocket.h"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cassert>

namespace LightIPC {

struct ProtocolHeader
{
	unsigned char hexspeak[4]; // 0xDEADC0DE
	unsigned int size;		   // message body size
};

UnixDomainSocket::UnixDomainSocket(const std::string &path, bool isOwner)
	: m_path(path)
	, m_isOwner(isOwner)
	, m_txSocketFd(-1)
	, m_rxSocketFd(-1)
	, m_isOpend(false)
	, m_limitSize(0xffffff) // 24bit(16.7Mb)
{
}

UnixDomainSocket::~UnixDomainSocket()
{
	CloseSocket();
}

/*
 client/server To send to and receive from each other
 tx/rx Socket connection by crossing (replace tx/rx on client side)
   server				  client
	send -> ${m_path}.tx -> recv 
	recv <- ${m_path}.rx <- send
*/
Result UnixDomainSocket::OpenSocket()
{
	Result result;
	if (IsOpend()) {
		return result;
	}

	// tx socket
	if (m_txSocketFd == -1) {
		std::string name(m_path);
		name += (m_isOwner ? ".tx" : ".rx");

		m_txSocketFd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
		m_txAddress.sun_family = AF_UNIX;
		::strcpy(m_txAddress.sun_path, name.c_str());
	}

	// rx socket
	if (m_rxSocketFd == -1) {
		std::string name(m_path);
		name += (m_isOwner ? ".rx" : ".tx");

		m_rxSocketFd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
		::unlink(name.c_str());

		m_rxAddress.sun_family = AF_UNIX;
		::strcpy(m_rxAddress.sun_path, name.c_str());

		int ret = ::bind(m_rxSocketFd, (sockaddr*)&m_rxAddress, sizeof(m_rxAddress));
		if (ret == -1) {
			result = Result::CreateError("open socket error [%s]", ::strerror(errno));
		}
		m_isOpend = (ret != -1);
	}
	return result;
}

void UnixDomainSocket::CloseSocket()
{
	if (!IsOpend()) {
		return;
	}

	if (m_txSocketFd != -1) {
		::close(m_txSocketFd);
		m_txSocketFd = -1;
	}
	if (m_rxSocketFd != -1) {
		::close(m_rxSocketFd);
		m_rxSocketFd = -1;
	}

	std::string name(m_path);
	name += (m_isOwner ? ".rx" : ".tx");
	::unlink(name.c_str());

	m_isOpend = false;
}

bool UnixDomainSocket::IsOpend()
{
	return m_isOpend;
}

// Transmission procedure
// At this time read, data that exceeds the specified size is discarded, so it is necessary to acquire it once.
// 1. ProtocolHeader Send
// 2. header Send
// 3. body Split send
Result UnixDomainSocket::Send(const ByteBuffer &header, const ByteBuffer &body)
{
	if (!IsOpend()) {
		return Result::CreateError("send socket error [%s]","socket closed");
	}

	if (header.Size() > 512) {
		return Result::CreateError("send header error [%s:%lu]"
				,"header too big size"
				, static_cast<unsigned long>(header.Size()));
	}

	size_t size = body.Size();

	if (0 < m_limitSize && m_limitSize < size) {
		return Result::CreateError("send header error [%s:%lu]"
				,"body too big size"
				, static_cast<unsigned long>(size));
	}

	ProtocolHeader ph;
	ph.hexspeak[0] = 0xDE; // dead code
	ph.hexspeak[1] = 0xAD;
	ph.hexspeak[2] = 0xC0;
	ph.hexspeak[3] = 0xDE;
	ph.size		   = static_cast<unsigned int>(size);

	ssize_t sentSize = 0;
	sentSize = ::sendto(m_txSocketFd, &ph, sizeof(ProtocolHeader), 0,(sockaddr*)&m_txAddress, sizeof(m_txAddress));
	if (sentSize != sizeof(ProtocolHeader)) {
		return Result::CreateError("send protocol header error [%s]",::strerror(errno));
	}

	sentSize = ::sendto(m_txSocketFd, header.Data().data(), header.Size(), 0,(sockaddr*)&m_txAddress, sizeof(m_txAddress));
	if (sentSize != static_cast<ssize_t>(header.Size())) {
		return Result::CreateError("send application header error [%s]",::strerror(errno));
	}

	if (size == 0) {
		return Result::CreateSuccess();
	}

	// send divided data
	static const size_t TRANSMIT_SIZE = 1024;
	const char *msg = body.Data().data();
	size_t txSize = 0;
	size_t transmitSize = 0;
	while (txSize != size) {
		transmitSize = size - txSize;
		if (transmitSize > TRANSMIT_SIZE) {
			transmitSize = TRANSMIT_SIZE;
		}
		sentSize = ::sendto(m_txSocketFd, msg + txSize, transmitSize, 0,(sockaddr*)&m_txAddress, sizeof(m_txAddress));
		if (sentSize == -1) {
			return Result::CreateError("send body error [%s]",::strerror(errno));
		}
		txSize += static_cast<size_t>(sentSize);
	}

	return Result::CreateSuccess();
}

Result UnixDomainSocket::Receive(ByteBuffer &outHeader, ByteBuffer &outBody)
{
	if (!IsOpend()) {
		return Result::CreateError("receive socket error [%s]","socket closed");
	}

	static const size_t RECEIVE_SIZE = 1024;
	char msg[RECEIVE_SIZE];

	ssize_t len = 0;
	len = ::recvfrom(m_rxSocketFd, msg, RECEIVE_SIZE, 0, NULL, NULL);
	if (len == -1 || len != sizeof(ProtocolHeader)) {
		return Result::CreateError("receive protocol header error [%s]",::strerror(errno));
	}

	ProtocolHeader ph;
	::memcpy(&ph, msg, sizeof(ProtocolHeader));

	if (ph.hexspeak[0] != 0xDE
	 || ph.hexspeak[1] != 0xAD
	 || ph.hexspeak[2] != 0xC0
	 || ph.hexspeak[3] != 0xDE
	) {
		return Result::CreateError("receive protocol header error [%s:0x%02X%02X%02X%02X]"
				,"invalid hexspeak",
				ph.hexspeak[0], ph.hexspeak[1], ph.hexspeak[2], ph.hexspeak[3]);
	}

	if (0 < m_limitSize && m_limitSize < ph.size) {
		return Result::CreateError("receive protocol header error [%s:%lu]"
				,"body too big size", ph.size);
	}

	len = ::recvfrom(m_rxSocketFd, msg, RECEIVE_SIZE, 0, NULL, NULL);
	if (len == -1 || len > 512) {
		return Result::CreateError("receive application header error [%s]",::strerror(errno));
	}
	outHeader = ByteBuffer(msg, len, len);

	unsigned long rsize = ph.size;
	char *req = msg;
	bool isAllocate = false;
	if (rsize > RECEIVE_SIZE) {
		req = new char[rsize];
		isAllocate = true;
	}

	// receive divided data
	size_t rxSize = 0;
	size_t receiveSize = 0;
	while (rxSize != rsize) {
		receiveSize = rsize - rxSize;
		if (receiveSize > RECEIVE_SIZE) {
			receiveSize = RECEIVE_SIZE;
		}
		len = ::read(m_rxSocketFd, req + rxSize, receiveSize);
		if (len == -1) {
			if (isAllocate) {
				delete [] req;
			}
			return Result::CreateError("receive body error [%s]",::strerror(errno));
		}
		rxSize += static_cast<size_t>(len);
	}

	outBody = ByteBuffer(req, rsize);
	if (isAllocate) {
		delete [] req;
	}

	return Result::CreateSuccess();
}

void UnixDomainSocket::SetLimitSize(unsigned int limit)
{
	m_limitSize = limit;
}

unsigned int UnixDomainSocket::LimitSize()
{
	return m_limitSize;
}

}
