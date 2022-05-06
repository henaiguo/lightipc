///////////////////////////////////////////////////////////
/// @file	MutexLock.h
/// @brief	Mutex Lock of
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_MUTEX_LOCK__
#define __LIGHT_IPC_MUTEX_LOCK__

#include "Mutex.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class MutexLock
/// @brief	Mutex Automate lock/unlock
///
/// To lock Mutex in the constructor and unlock it in the destructor
/// Use in stack area (local variable)
///
///////////////////////////////////////////////////////////
class MutexLock
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	mutex Mutex*
	///////////////////////////////////////////////////////////
	MutexLock(Mutex *mutex);

	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	mutex Mutex*
	/// @param[in]	isYieldEnd True if the thread that called after the destructor releases the CPU usage right
	///////////////////////////////////////////////////////////
	MutexLock(Mutex *mutex, bool isYieldEnd);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~MutexLock();

	///////////////////////////////////////////////////////////
	/// @brief		Calling thread waits for lock
	/// @param[in]	None
	/// @return		None
	/// @note		Mutex is temporarily unlocked while waiting
	///				When getting up (when exiting Wait()), Mutex lock will be acquired.
	///////////////////////////////////////////////////////////
	void Wait();

	///////////////////////////////////////////////////////////
	/// @brief		Wake up one of the waiting threads
	/// @param[in]	None
	/// @return		None
	/// @note		When multiple threads are waiting, you cannot specify which thread
	///////////////////////////////////////////////////////////
	void Signal();

	///////////////////////////////////////////////////////////
	/// @brief		Wake up all waiting threads
	/// @param[in]	None
	/// @return		None
	/// @note		You cannot specify in what order threads will occur
	///////////////////////////////////////////////////////////
	void Broadcast();

private:
	/// Mutex
	Mutex *m_mutex;

	/// Whether threads calling after destructor give up CPU usage
	bool m_isYieldEnd;

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibited
	///////////////////////////////////////////////////////////
	MutexLock(const MutexLock &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	MutexLock& operator=(const MutexLock &src);
};

}
#endif
