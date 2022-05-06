///////////////////////////////////////////////////////////
/// @file	UnixDomainSocketServer.h
/// @brief	UNIX Domain socket server
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_UNIX_DOMAIN_SOCKET_SERVER__
#define __LIGHT_IPC_UNIX_DOMAIN_SOCKET_SERVER__

#include "UnixDomainSocket.h"
#include "Mutex.h"
#include "Thread.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class	IRequestReceiver
/// @brief	Request receiving interface
/// 
/// - Called when a request message from the client is received
/// - Implement this interface to implement the process when a request message is received
/// - The processing when received should be finished promptly
///
///////////////////////////////////////////////////////////
class IRequestReceiver
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~IRequestReceiver() {};

	///////////////////////////////////////////////////////////
	/// @brief		Implement request reception process
	///
	/// Promptly end the process to wait for the next reception until the end of this process
	///
	/// @param[in]	request received data
	/// @param[out]	response Response data
	/// @note		Wait for next reception until sending response
	/// @note		Return appropriate response data according to the received data
	///////////////////////////////////////////////////////////
	virtual void Received(ByteBuffer &request, ByteBuffer &response) = 0;

	///////////////////////////////////////////////////////////
	/// @brief		Called when an error occurs when receiving a request
	/// @param[in]	Result result contents
	/// @note		Default does nothing
	///////////////////////////////////////////////////////////
	virtual void ReceiveError(const Result &result) {};

	///////////////////////////////////////////////////////////
	/// @brief		Called when an error occurs while sending a response
	/// @param[in]	Result result contents
	/// @note		Default does nothing
	///////////////////////////////////////////////////////////
	virtual void ResponseError(const Result &result) {};
};

///////////////////////////////////////////////////////////
/// @class	UnixDomainSocketServer
/// @brief	UNIX Domain socket server
/// 
/// - UnixDomainSocket Classes that inherit
/// - IRunnable is implemented because the request reception process is internally threaded.
/// - Receives a request from the connection partner (client), performs some processing,
///   Implement IRequestReceiver to send response
/// - Call Notify() if you want to send a message to the other party (client).
///
///////////////////////////////////////////////////////////
class UnixDomainSocketServer : public UnixDomainSocket, public IRunnable
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	path The file path representing the socket
	/// @note		path Needs to set the same path as the communication partner (client)
	///////////////////////////////////////////////////////////
	UnixDomainSocketServer(const std::string &path);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~UnixDomainSocketServer();

	///////////////////////////////////////////////////////////
	/// @brief		IRequestReceiver To set
	/// @param[in]	receiver IRequestReceiver
	/// @note		
	///////////////////////////////////////////////////////////
	void SetReceiver(IRequestReceiver *receiver);

	///////////////////////////////////////////////////////////
	/// @brief		Start request reception process from the connection partner (client)
	/// @param[in]	isBlock Stop() True if you want to block with this method until is called
	/// @note		isBlock Is true, another thread will call Stop()
	///////////////////////////////////////////////////////////
	void Start(bool isBlock);

	///////////////////////////////////////////////////////////
	/// @brief		Stop the request reception process from the connection partner (client)
	/// @note		
	///////////////////////////////////////////////////////////
	void Stop();

	///////////////////////////////////////////////////////////
	/// @brief		Notify the other party (client) of the message
	/// @param[in]	update Message to notify
	/// @return		Result When it fails, the error content is set to Error
	/// @note		There is no response because it is a one-way message transmission
	///////////////////////////////////////////////////////////
	Result Notify(ByteBuffer &update);

	///////////////////////////////////////////////////////////
	/// @brief		Send a ping to the connection partner (client)
	/// @param[in]	None
	/// @return		Result When it fails, the error content is set to Error
	/// @note		If successful, the client is ready to send and receive
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
	Thread m_receiveThread;

	/// IRequestReceiver
	IRequestReceiver *m_requestReceiver;

	/// Activated state
	bool m_isActive;
};
}

#endif
