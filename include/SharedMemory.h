///////////////////////////////////////////////////////////
/// @file	SharedMemory.h
/// @brief	Shared memory
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_SHARED_MEMORY__
#define __LIGHT_IPC_SHARED_MEMORY__

#include <string>
#include <cassert>
#include "Semaphore.h"
#include "Result.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class SharedMemory
/// @brief	POSIX Shared memory
/// 
/// - Manage (create/access/delete) a memory area that can be shared by multiple processes
/// - Map the user-defined structure (only POD[Plain Old Data] type is supported) to the shared memory area.
/// - Memory area has a fixed length
/// - Providing exclusive control and synchronization by using Semaphore for each shared memory
///
///////////////////////////////////////////////////////////
class SharedMemory {
public:
	///////////////////////////////////////////////////////////
	/// @brief		Check if the shared memory with the specified name exists
	/// @return		Result
	/// @note		name Must start with'/' eg) "/shared_memory1"
	///////////////////////////////////////////////////////////
	static Result Exist(const std::string &name);
	
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	name name
	/// @param[in]	size Shared memory size
	/// @param[in]	isOwner Ownership
	/// @note		name Must start with'/' eg) "/shared_memory1"
	/// @note		isOwner Is true, create/delete shared memory
	/// 			false In case of, use the created shared memory
	///////////////////////////////////////////////////////////
	SharedMemory(const std::string &name, size_t size, bool isOwner);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~SharedMemory();

	///////////////////////////////////////////////////////////
	/// @brief		Get name
	/// @return		name
	/// @note
	///////////////////////////////////////////////////////////
	const std::string &Name() const;

	///////////////////////////////////////////////////////////
	/// @brief		Get shared memory area with specified type
	/// @return		Template T pointer
	/// @note		The size specified in the constructor >= sizeof (template T)
	///////////////////////////////////////////////////////////
	template<typename T> T *Data()
	{
		return reinterpret_cast<T *>(m_memoryMap);
	}

	///////////////////////////////////////////////////////////
	/// @brief		Wait until you lock (acquire)
	/// @note		Another process or thread that has locked
	/// 			Post() Wait until you unlock with
	/// @note		Lock in the order in which each process or thread did Wait()
	/// 			Note that you may not always win
	///////////////////////////////////////////////////////////
	void Wait();

	///////////////////////////////////////////////////////////
	/// @brief		unlock
	/// @note		Wait() Another process or thread waiting on
	///				Be evoked and be able to lock
	///////////////////////////////////////////////////////////
	void Post();

private:
	/// Name
	std::string m_name;

	/// Ownership
	bool m_isOwner;

	/// Shared memory area
	void *m_memoryMap;

	/// Size
	size_t m_size;

	/// Semaphore
	Semaphore *m_semaphore;

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibited
	///////////////////////////////////////////////////////////
	SharedMemory(const SharedMemory &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	SharedMemory& operator=(const SharedMemory &src);
};
}
#endif
