///////////////////////////////////////////////////////////
/// @file	SharedLock.h
/// @brief	Shared memory lock
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_SHARED_LOCK__
#define __LIGHT_IPC_SHARED_LOCK__

#include "SharedMemory.h"
#include "Thread.h"

namespace LightIPC {

///////////////////////////////////////////////////////////
/// @class SharedLock
/// @brief	Automate locking/unlocking shared memory
/// 
/// Semaphore Lock using
/// To lock SharedMemory in the constructor and unlock it in the destructor
/// Use in stack area (local variable)
///
/// How to use
///   struct UserDefinedStruct
///   {
///       int    a;
///       bool   b;
///       double c;
///   };
///
///   SharedMemory *shm = ...  // UserDefinedStruct SharedMemory for is already created
///     :
///
///   // { To lock UserDefinedStruct in the block scope from to
///   {
///      SharedLock<UserDefinedStruct> l(shm);
///      l->x = 1; 
///      l->y = true; 
///      l->z = 1.23; 
///   }
///
///   // When locking UserDefinedStruct in a function
///   void function(SharedMemory *shm) {
///      SharedLock<UserDefinedStruct> l(shm);
///      l->x = 1; 
///      l->y = true; 
///      l->z = 1.23; 
///   }
///
///////////////////////////////////////////////////////////
template <typename T>
class SharedLock
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	memory SharedMemory
	/// @note		When the constructor ends, memory becomes locked
	///////////////////////////////////////////////////////////
	SharedLock(SharedMemory *memory)
		: m_memory(memory)
		, m_data(memory->Data<T>())
		, m_isYieldEnd(false)
	{
		m_memory->Wait();
	}

	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	memory SharedMemory
	/// @param[in]	isYieldEnd True if the destructor gives up the CPU
	/// @note		When the constructor ends, memory becomes locked
	///////////////////////////////////////////////////////////
	SharedLock(SharedMemory *memory, bool isYieldEnd)
		: m_memory(memory)
		, m_data(memory->Data<T>())
		, m_isYieldEnd(isYieldEnd)
	{
		m_memory->Wait();
	}

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	/// @note		When the destructor ends, the memory is unlocked.
	/// @note		isYieldEnd Yields CPU when true and allocates CPU to other processes and threads
	///////////////////////////////////////////////////////////
	~SharedLock()
	{
		m_memory->Post();
		if (m_isYieldEnd) {
			Thread::Yield();
		}
	}

	///////////////////////////////////////////////////////////
	/// @brief		Arrow operator override
	/// 
	/// SharedMemory Returns the data that
	/// 
	///////////////////////////////////////////////////////////
	T *operator->() const
	{
		return m_data;
	}

private:
	/// SharedMemory
	SharedMemory *m_memory;

	/// SharedMemory Data that
	T *m_data;

	/// Whether to abandon CPU at destructor
	bool m_isYieldEnd;

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibited
	///////////////////////////////////////////////////////////
	SharedLock(const SharedLock &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	SharedLock& operator=(const SharedLock &src);
};
}

#endif
