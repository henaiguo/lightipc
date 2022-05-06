///////////////////////////////////////////////////////////
/// @file	Mutex.h
/// @brief	Mutex
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_MUTEX__
#define __LIGHT_IPC_MUTEX__

#include <pthread.h>

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class Mutex
/// @brief	POSIX Mutex
/// 
/// To ensure atomicity in the critical section,
/// Providing exclusive control and synchronization between threads
/// 
///////////////////////////////////////////////////////////
class Mutex
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	None
	///////////////////////////////////////////////////////////
	Mutex();

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~Mutex();

	///////////////////////////////////////////////////////////
	/// @brief		Wait until the calling thread acquires the lock
	/// @param[in]	None
	/// @return		None
	/// @note		Lock() and Unlock() should always be treated as a pair to avoid deadlock
	///////////////////////////////////////////////////////////
	void Lock();

	///////////////////////////////////////////////////////////
	/// @brief		The calling thread releases the lock
	/// @param[in]	None
	/// @return		None
	/// @note		Lock() and Unlock() should always be treated as a pair to avoid deadlock
	///////////////////////////////////////////////////////////
	void Unlock();

	///////////////////////////////////////////////////////////
	/// @brief		Wait for the calling thread using the condition variables held by this class
	/// @param[in]	None
	/// @return		None
	/// @note		It is necessary to acquire (Lock()) the lock of this class.
	///				While waiting, Mutex will be unlocked temporarily.
	///				When getting up (when exiting Wait()), Mutex lock will be acquired.
	///////////////////////////////////////////////////////////
	void ConditionWait();

	///////////////////////////////////////////////////////////
	/// @brief		Wake up (send a signal) a thread waiting on the condition variable held by this class
	/// @param[in]	None
	/// @return		None
	/// @note		It is necessary to acquire (Lock()) the lock of this class.
	///				When multiple threads are in Wait(), it is not possible to specify which thread
	///////////////////////////////////////////////////////////
	void ConditionSignal();

	///////////////////////////////////////////////////////////
	/// @brief		Wake up (signal) all threads waiting on the condition variables held by this class
	/// @param[in]	None
	/// @return		None
	/// @note		It is necessary to acquire (Lock()) the lock of this class.
	///				You cannot specify in what order threads will occur
	///////////////////////////////////////////////////////////
	void ConditionBroadcast();

private:
	/// POSIX Mutex
	::pthread_mutex_t m_mutex;

	/// Condition variable
	::pthread_cond_t  m_condition;

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibited
	///////////////////////////////////////////////////////////
	Mutex(const Mutex &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	Mutex& operator=(const Mutex &src);
};

}
#endif
