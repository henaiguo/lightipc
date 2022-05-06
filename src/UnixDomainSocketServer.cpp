#include "UnixDomainSocketServer.h"

#include <unistd.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cassert>
#include "MutexLock.h"

namespace LightIPC {

UnixDomainSocketServer::UnixDomainSocketServer(const std::string &path)
	: UnixDomainSocket(path, true)
	, m_mutex()
	, m_receiveThread()
	, m_requestReceiver(NULL)
	, m_isActive(false)
{
	OpenSocket();
}

UnixDomainSocketServer::~UnixDomainSocketServer()
{
	Stop();
	CloseSocket();
}

void UnixDomainSocketServer::SetReceiver(IRequestReceiver *receiver)
{
	m_requestReceiver = receiver;
}

void UnixDomainSocketServer::Run()
{
	Result result;
	ByteBuffer header;
	ByteBuffer request;
	ByteBuffer response;
	while (m_isActive) {
		// Block here until data is received
		result = Receive(header, request);
		if (result.IsError()) {
			if (m_requestReceiver) {
				m_requestReceiver->ReceiveError(result);
			}
			continue;
		}
		if (!m_isActive) {
			break;
		}
		
		unsigned int requestType;
		header.Value(requestType);
		if (requestType == 2) { // PING
			header.SetPosition(0);
			response.Append("OK");
		}
		else {
			if (m_requestReceiver) {
				m_requestReceiver->Received(request,response);
			}
		}

		// Synchronous processing during transmission
		{
			MutexLock lock(&m_mutex);
			result = Send(header, response);
			if (result.IsError()) {
				if (m_requestReceiver && requestType != 2) {
					m_requestReceiver->ResponseError(result);
				}
			}
		}

		response.Clear();
		request.Clear();
	}
	m_isActive = false;
}

void UnixDomainSocketServer::Start(bool isBlock)
{
	m_receiveThread.SetRunner(this, NULL);
	m_receiveThread.SetName("receiveThread");
	m_isActive = true;
	m_receiveThread.Start();
	if (isBlock) {
		m_receiveThread.Join();
	}
}

void UnixDomainSocketServer::Stop()
{
	m_isActive = false;
	m_receiveThread.Cancel();
	m_receiveThread.Join();
}

Result UnixDomainSocketServer::Notify(ByteBuffer &update)
{
	// Synchronous processing during transmission
	MutexLock lock(&m_mutex);
	ByteBuffer header;
	unsigned int requestType = 1;
	header.Append(requestType);
	return Send(header, update); 
}

Result UnixDomainSocketServer::Ping()
{
	MutexLock lock(&m_mutex);
	ByteBuffer header;
	ByteBuffer body;
	unsigned int requestType = 3; // PING from Server
	header.Append(requestType);
	body.Append("PING");
	return Send(header, body); 
}

}