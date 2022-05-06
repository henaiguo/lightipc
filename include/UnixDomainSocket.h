///////////////////////////////////////////////////////////
/// @file	UnixDomainSocket.h
/// @brief	UNIX Domain socket
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_UNIX_DOMAIN_SOCKET__
#define __LIGHT_IPC_UNIX_DOMAIN_SOCKET__

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "ByteBuffer.h"
#include "Result.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class UnixDomainSocket
/// @brief	UNIX Use datagrams with domain sockets
/// 
///  - Used for one-to-one interprocess communication between the same hosts
///  - TCP Faster than connection
///  - Data is divided by transmission unit
///  - Data will not disappear
///  - The order of data does not get out of order
///  - Connection error occurs when there is no connection destination
/// 
///  [ Send/Receive message data structure]
///               Top                                            Bottom
///               0                                                     n
///               +-----------------+-----------------+-----------------+
///   Category    | Protocol Header |     Header      |      Body       |
///               +-----------------+-----------------+-----------------+
///   Data Type   |    ------       |   ByteBuffer    |   ByteBuffer    |
///               +-----------------+-----------------+-----------------+
///   Owner       |    Framework    |   Application   |   Application   |   
///               +-----------------+-----------------+-----------------+
///   Data Length |  Fixed Length   | Variable Length | Variable Length |
///               |    8 byte       |   < 512 byte    | n byte < limit  |
///               +-----------------+-----------------+-----------------+
///
///////////////////////////////////////////////////////////
class UnixDomainSocket
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	path The file path representing the socket
	/// @param[in]	isOwner Ownership
	/// @note		path Must be a file-creatable path
	/// @note		1 One-to-one communication same path and isOwner is true and false
	/// 			Establish a communication connection with (not meant to be ownership)
	///////////////////////////////////////////////////////////
	UnixDomainSocket(const std::string &path, bool isOwner);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~UnixDomainSocket();

	///////////////////////////////////////////////////////////
	/// @brief		Open socket
	/// @return		Result When it fails, the error content is set to Error
	///////////////////////////////////////////////////////////
	Result OpenSocket();

	///////////////////////////////////////////////////////////
	/// @brief		Close socket
	///////////////////////////////////////////////////////////
	void CloseSocket();

	///////////////////////////////////////////////////////////
	/// @brief		Check if the socket is open
	/// @return		true When the socket is open
	///////////////////////////////////////////////////////////
	bool IsOpend();

	///////////////////////////////////////////////////////////
	/// @brief		Send data to the other party
	/// @param[in]	header Header data
	/// @param[in]	body  Body data
	/// @return		Result When it fails, the error content is set to Error
	/// @note		header The contents of body are set by the user
	///////////////////////////////////////////////////////////
	Result Send(const ByteBuffer &header, const ByteBuffer &body);

	///////////////////////////////////////////////////////////
	/// @brief		Receive data from the other party
	/// @param[out]	header Header data
	/// @param[out]	body  Body data
	/// @return		Result When it fails, the error content is set to Error
	/// @note		The connection partner receives the header and body sent ().
	///////////////////////////////////////////////////////////
	Result Receive(ByteBuffer &outHeader, ByteBuffer &outBody);

	///////////////////////////////////////////////////////////
	/// @brief		Specify the maximum send/receive data size
	/// @param[in]	limit Maximum send/receive data size
	/// @note		0: Unlimited, default:0xffffff(16.7Mb)
	///////////////////////////////////////////////////////////
	void SetLimitSize(unsigned int limit);

	///////////////////////////////////////////////////////////
	/// @brief		Get maximum send/receive data size
	/// @note		0 Represents unlimited
	///////////////////////////////////////////////////////////
	unsigned int LimitSize();

private:
	/// File Path
	std::string m_path;

	/// Data transmission/reception file path switching flag
	bool m_isOwner;

	/// Socket file descriptor for sending
	int m_txSocketFd;

	/// Socket file descriptor for reception
	int m_rxSocketFd;

	/// UNIX domain socket address structure for sending
	sockaddr_un m_txAddress;

	/// Incoming UNIX domain socket address structure
	sockaddr_un m_rxAddress;

	/// Socket open state
	bool m_isOpend;

	/// Maximum send/receive data size
	unsigned int m_limitSize;
};
}

#endif
