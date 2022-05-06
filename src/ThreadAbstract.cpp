#include "ThreadAbstract.h"
#include "MutexLock.h"

namespace LightIPC {
ThreadAbstract::ThreadAbstract()
    : m_freq(100),m_exit(false)
{
}

ThreadAbstract::~ThreadAbstract()
{
    Finalize();
}

void ThreadAbstract::Initialize(unsigned int _freq)
{
    m_freq = _freq;
}

void ThreadAbstract::Finalize()
{
    StopThread(false);
}

void ThreadAbstract::Run()
{
    // On thread start
    if (!OnStart()) return;

    // On thread looping
    while (true)
    {
        if (m_exit) break;
        if (!OnLooping()) break;

        Thread::MilliSleep(m_freq);
        Thread::Yield();
    }
  
}

void ThreadAbstract::Cleanup()
{
    // On thread stop
    OnStop();
}

void ThreadAbstract::StartThread(const std::string& _name, bool _isBlock)
{
    if (m_thread.IsActive()) return;
    
    m_thread.SetRunner(static_cast<IRunnable*>(this), NULL);
	m_thread.SetName(_name);
    m_thread.Start();

    if (_isBlock) m_thread.Join();
}

void ThreadAbstract::StopThread(bool _wait)
{
    if (!m_thread.IsActive()) return;

    if (_wait) m_exit = true;
    else m_thread.Cancel();
}

} // namespace manager
