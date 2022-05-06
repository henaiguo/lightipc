#include "MutexLock.h"

#include "Thread.h"

namespace LightIPC {

MutexLock::MutexLock(Mutex *mutex)
	: m_mutex(mutex)
	, m_isYieldEnd(false)
{
	m_mutex->Lock();
}

MutexLock::MutexLock(Mutex *mutex, bool isYieldEnd)
	: m_mutex(mutex)
	, m_isYieldEnd(isYieldEnd)
{
	m_mutex->Lock();
}

MutexLock::~MutexLock()
{
	m_mutex->Unlock();
	if (m_isYieldEnd) {
		Thread::Yield();
	}
}

void MutexLock::Wait()
{
	m_mutex->ConditionWait();
}

void MutexLock::Signal()
{
	m_mutex->ConditionSignal();
}

void MutexLock::Broadcast()
{
	m_mutex->ConditionBroadcast();
}

}
