///////////////////////////////////////////////////////////
/// @file	SharedMemoryContext.h
/// @brief	Shared memory context
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_SHARED_MEMORY_CONTEXT__
#define __LIGHT_IPC_SHARED_MEMORY_CONTEXT__

#include <string>
#include <map>
#include "SharedMemory.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class SharedMemoryContext
/// @brief	Manage shared memory creation and destruction
/// 
///////////////////////////////////////////////////////////
class SharedMemoryContext
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	///////////////////////////////////////////////////////////
	SharedMemoryContext();

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///
	/// Bind() SharedMemory created by is automatically released
	///
	///////////////////////////////////////////////////////////
	virtual ~SharedMemoryContext();

	///////////////////////////////////////////////////////////
	/// @brief		Get SharedMemory by specifying POD type data to bind
	/// @param[in]	name name
	/// @param[in]	isOwner Ownership
	/// @note		name Must start with'/' eg) "/shared_memory1"
	/// @note		isOwner Is true, create if shared memory does not exist.
	/// 			false If the shared memory does not exist, returns a null if it does not exist.
	/// 			SharedMemory Is released by SharedMemoryContext
	///				User should not delete the acquired SharedMemory
	/// 
	/// 			Example) SharedMemory *sm = context->bind<MyStruct>("/shared_memory", false);
	///////////////////////////////////////////////////////////
	template<typename T> SharedMemory *Bind(const std::string &name, bool isOwner)
	{
		return bindSharedMemory(name, sizeof(T), isOwner);
	}

private:
	/// SharedMemory List
	std::map<const std::string, SharedMemory *> m_sharedMemories;

	///////////////////////////////////////////////////////////
	/// @brief		Bind shared memory
	/// @param[in]	name name
	/// @param[in]	size Shared memory size
	/// @param[in]	isOwner Ownership
	/// @note
	///////////////////////////////////////////////////////////
	SharedMemory *bindSharedMemory(const std::string &name, size_t size, bool isOwner);

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibition
	///////////////////////////////////////////////////////////
	SharedMemoryContext(const SharedMemoryContext &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	SharedMemoryContext& operator=(const SharedMemoryContext &src);
};

}
#endif
