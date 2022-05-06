///////////////////////////////////////////////////////////
/// @file	UnixDomainSocketClient.h
/// @brief	UNIX Domain socket client
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_UNIX_DOMAIN_SOCKET_CLIENT__
#define __LIGHT_IPC_UNIX_DOMAIN_SOCKET_CLIENT__

#include "UnixDomainSocket.h"
#include "Mutex.h"
#include "Thread.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class	INotifyReceiver
/// @brief	Notification message receiving interface
/// 
/// - Called when a notification message is received from the server
/// - Implement this interface to implement the process when a notification message is received
/// - The processing when received should be finished promptly
///
///////////////////////////////////////////////////////////
class INotifyReceiver
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~INotifyReceiver() {};

	///////////////////////////////////////////////////////////
	/// @brief		Implement notification reception process
	///
	/// Promptly end the process to wait for the next reception until the end of this process
	///
	///////////////////////////////////////////////////////////
	virtual void ReceiveNotify(ByteBuffer &update) = 0;
};


///////////////////////////////////////////////////////////
/// @class	UnixDomainSocketClient
/// @brief	UNIX Domain socket client
/// 
/// - UnixDomainSocket Classes that inherit
/// - IRunnable is implemented because the receiving process is internally threaded.
/// - Send a request to the connection partner (server), receive a response, and perform some processing
/// - Implement INotifyReceiver to receive notification from the other party (server)
///
///////////////////////////////////////////////////////////
class UnixDomainSocketClient : public UnixDomainSocket, public IRunnable
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	path The file path representing the socket
	/// @note		path Needs to set the same path as the communication partner (server)
	///////////////////////////////////////////////////////////
	UnixDomainSocketClient(const std::string &path);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~UnixDomainSocketClient();

	///////////////////////////////////////////////////////////
	/// @brief		INotifyReceiver To set
	/// @param[in]	receiver INotifyReceiver
	/// @note		
	///////////////////////////////////////////////////////////
	void SetNotifyReceiver(INotifyReceiver *receiver);

	///////////////////////////////////////////////////////////
	/// @brief		Send a request to the other party (server) and receive a response
	/// @param[in]	request Transmission data
	/// @param[out]	response received data
	/// @return		Result When it fails, the error content is set to Error
	/// @note		Wait until you receive the response
	///////////////////////////////////////////////////////////
	Result SendReceive(ByteBuffer &request, ByteBuffer &response);

	///////////////////////////////////////////////////////////
	/// @brief		Send a ping to the connection partner (server)
	/// @param[in]	None
	/// @return		Result When it fails, the error content is set to Error
	/// @note		If successful, the server is ready to receive
	///////////////////////////////////////////////////////////
	Result Ping();

	///////////////////////////////////////////////////////////
	/// @brief		implements IRunnable::Run()
	/// @note		Do not call this method
	///////////////////////////////////////////////////////////
	void Run();

private:
	/// Mutex to synchronize sending process
	Mutex m_mutex;

	/// Reception processing Thread
	Thread m_responseThread;

	/// INotifyReceiver
	INotifyReceiver *m_receiver;

	/// Activated state
	bool m_isActive;

	/// Mutex for reception
	Mutex m_responseMutex;

	/// Received data arrival status
	bool m_responseArrived;

	/// Received error information
	Result m_isResponseError;

	/// Receive header information
	ByteBuffer m_responseHeader;

	/// Received body information
	ByteBuffer m_response;

	///////////////////////////////////////////////////////////
	/// @brief		Start response data service
	/// @param[in]	isBlock Block here if isBlock is true
	/// @note
	///////////////////////////////////////////////////////////
	void start(bool isBlock);

	///////////////////////////////////////////////////////////
	/// @brief		Stop service
	/// @param[in]	None
	/// @note
	///////////////////////////////////////////////////////////
	void stop();

	///////////////////////////////////////////////////////////
	/// @brief		Send a request to the other party (server) and receive a response
	/// @param[in]	request Transmission data
	/// @param[out]	response received data
	/// @param[in]	requestType Transmission data type
	/// @return		Result When it fails, the error content is set to Error
	/// @note
	///////////////////////////////////////////////////////////
	Result privateSendReceive(ByteBuffer &request, ByteBuffer &response, unsigned int requestType);
};
}

#endif
