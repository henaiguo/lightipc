#include "UnixDomainSocketClient.h"

#include <unistd.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cassert>
#include "MutexLock.h"

namespace LightIPC {

UnixDomainSocketClient::UnixDomainSocketClient(const std::string &path)
	: UnixDomainSocket(path, false)
	, m_mutex()
	, m_responseThread()
	, m_receiver(NULL)
	, m_isActive(false)
	, m_responseArrived(false)
	, m_isResponseError()
{
	OpenSocket();
	start(false);
}

UnixDomainSocketClient::~UnixDomainSocketClient()
{
	stop();
	CloseSocket();
}

void UnixDomainSocketClient::SetNotifyReceiver(INotifyReceiver *receiver)
{
	m_receiver = receiver;
}

Result UnixDomainSocketClient::SendReceive(ByteBuffer &request, ByteBuffer &response)
{
	return privateSendReceive(request, response, 0);
}

Result UnixDomainSocketClient::privateSendReceive(ByteBuffer &request, ByteBuffer &response, unsigned int requestType)
{
	if (!IsOpend()) {
		return Result::CreateError("closed socket");
	}
	// Synchronous processing during transmission and reception
	MutexLock lock(&m_mutex);
	// Access to received information is synchronous
	MutexLock responseLock(&m_responseMutex);

	// Send request
	ByteBuffer header;
	header.Append(requestType);
	Result result = Send(header, request);
	if (result.IsError()) {
		return result;
	}

	// Response waiting process
	{
		// Clear received information
		m_responseArrived = false;
		m_isResponseError = Result::CreateSuccess();
		m_responseHeader.Clear();
		m_response.Clear();

		// Wait until response data is received
		// wait() When exiting, it exits with the lock applied again (it is the same as before waiting())
		while (!m_responseArrived && m_isActive) {
			responseLock.Wait(); // Internally unlock and wait
		}

		// When inactive, do not care about subsequent states
		if (!m_isActive) {
			return Result::CreateError("currently innactive");
		}

		if (m_isResponseError) {
			return m_isResponseError;
		}

		// Check the received data because it has already been received
		unsigned int responseType;
		m_responseHeader.Value(responseType);
		if (responseType == 0 || responseType == 2) {
			response = m_response; // copy
		}

		// Clear received information
		m_responseArrived = false;
		m_isResponseError = Result::CreateSuccess();
		m_responseHeader.Clear();
		m_response.Clear();
	}

	return Result::CreateSuccess();
}

Result UnixDomainSocketClient::Ping()
{
	ByteBuffer req;
	ByteBuffer res;
	req.Append("PING");
	return privateSendReceive(req,res, 2);
}

// Since there is an asynchronous notification from server , receive processing is performed by thread processing
void UnixDomainSocketClient::Run()
{
	ByteBuffer header;
	ByteBuffer response;
	unsigned int responseType;
	while (m_isActive) {
		m_isResponseError = Receive(header, response);
		if (m_isResponseError) {
			// Receive error
			MutexLock responseLock(&m_responseMutex);
			m_responseArrived = true;
			responseLock.Signal();
			continue;
		}

		if (!m_isActive) {
			break;
		}
		
		header.Value(responseType);
		header.SetPosition(0);
		if (responseType == 0) { // request/response message
			MutexLock responseLock(&m_responseMutex);
			m_responseArrived = true;
			m_responseHeader = header;
			m_response = response;
			responseLock.Signal();
		}
		else if (responseType == 1){ // notify message
			if (m_receiver) {
				m_receiver->ReceiveNotify(response);
			}
		}
		else if (responseType == 2){ // PING from Client(Response)
			MutexLock responseLock(&m_responseMutex);
			m_responseArrived = true;
			m_responseHeader = header;
			m_response = response;
			responseLock.Signal();
		}
		else if (responseType == 3){ // PING from Server(Notify)
			// throw away
		}
		else {
			// Discard (do not come here because it is a receive error)
		}

		response.Clear();
	}
	m_isActive = false;
}

void UnixDomainSocketClient::start(bool isBlock)
{
	m_responseThread.SetRunner(this, NULL);
	m_responseThread.SetName("responseThread");
	m_isActive = true;
	m_responseThread.Start();
	if (isBlock) {
		m_responseThread.Join();
	}
}

void UnixDomainSocketClient::stop()
{
	m_isActive = false;
	m_responseThread.Cancel();
	m_responseThread.Join();
}

}
