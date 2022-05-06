#include "Mutex.h"

#include <pthread.h>
#include <cstdio>
#include <cassert>
#include "Thread.h"

namespace LightIPC {

Mutex::Mutex()
{
	::pthread_mutex_init(&m_mutex, NULL);
	::pthread_cond_init(&m_condition, NULL);
}

Mutex::~Mutex()
{
	::pthread_mutex_destroy(&m_mutex);
	::pthread_cond_destroy(&m_condition);
}

void Mutex::Lock()
{
	::pthread_mutex_lock(&m_mutex);
}

void Mutex::Unlock()
{
	::pthread_mutex_unlock(&m_mutex);
}

void Mutex::ConditionWait()
{
	::pthread_cond_wait(&m_condition, &m_mutex);
}

void Mutex::ConditionSignal()
{
	::pthread_cond_signal(&m_condition);
}

void Mutex::ConditionBroadcast()
{
	::pthread_cond_broadcast(&m_condition);
}

}
