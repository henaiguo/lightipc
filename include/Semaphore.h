///////////////////////////////////////////////////////////
/// @file	Semaphore.h
/// @brief	semaphore
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_SEMAPHORE__
#define __LIGHT_IPC_SEMAPHORE__

#include <semaphore.h>
#include <string>
#include "Result.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class Semaphore
/// @brief	POSIX Named semaphore
/// 
/// To ensure atomicity in the critical section,
/// Provide exclusive control and synchronization mechanism between processes (threads are also possible)
/// 
///////////////////////////////////////////////////////////
class Semaphore {
public:
	///////////////////////////////////////////////////////////
	/// @brief		Check if a semaphore with the specified name exists
	/// @return		Result
	/// @note		name Must start with'/' eg) "/semahore1"
	///////////////////////////////////////////////////////////
	static Result Exist(const std::string &name);

	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	name name
	/// @param[in]	isOwner Ownership
	/// @note		name Must start with'/' eg) "/semahore1"
	/// @note		isOwner Is true, create/delete semaphore
	/// 			false In case of, use the created semaphore
	///////////////////////////////////////////////////////////
	Semaphore(const std::string &name, bool isOwner);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	/// @note
	///////////////////////////////////////////////////////////
	virtual ~Semaphore();

	///////////////////////////////////////////////////////////
	/// @brief		Get name
	/// @return		name
	/// @note
	///////////////////////////////////////////////////////////
	const std::string &Name() const;

	///////////////////////////////////////////////////////////
	/// @brief		Wait until you lock (acquire) the semaphore
	/// @note		Another process or thread that locked the semaphore
	/// 			Post() Wait until you unlock with
	/// @note		Lock in the order in which each process or thread did Wait()
	/// 			Note that you may not always win
	///////////////////////////////////////////////////////////
	void Wait();

	///////////////////////////////////////////////////////////
	/// @brief		Unlock semaphore
	/// @note		Wait() Another process or thread waiting on
	///				You will be awakened and will be able to lock the semaphore
	///////////////////////////////////////////////////////////
	void Post();

private:
	/// Name
	std::string m_name;

	/// Ownership
	bool m_isOwner;

	/// POSIX semaphore
	sem_t *m_semaphore;

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibited
	///////////////////////////////////////////////////////////
	Semaphore(const Semaphore &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	Semaphore& operator=(const Semaphore &src);
};
}
#endif
