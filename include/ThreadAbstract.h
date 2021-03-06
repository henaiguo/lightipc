///////////////////////////////////////////////////////////
/// @file	ThreadAbstract.h
/// @brief	Abstract thread
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_THREAD_ABSTRACT_
#define __LIGHT_IPC_THREAD_ABSTRACT_

#include "Thread.h"
#include <string>

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @file	ThreadAbstract
/// @brief	Abstract thread
/// @note   Thread inheritance class
///////////////////////////////////////////////////////////
class ThreadAbstract : public IRunnable
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		Constructor
	/// @return		None
	/// @note
	///////////////////////////////////////////////////////////
	ThreadAbstract();

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	/// @return		None
	/// @note
	///////////////////////////////////////////////////////////
	virtual ~ThreadAbstract();

	///////////////////////////////////////////////////////////
	/// @brief		Initial processing
	/// @param[in]	_freq Thread cycle frequency(ms)
	/// @return		void
	/// @note
	///////////////////////////////////////////////////////////
	virtual void Initialize(unsigned int _freq = 100);

	///////////////////////////////////////////////////////////
	/// @brief		Perform termination processing
	/// @return		None
	/// @note
	///////////////////////////////////////////////////////////
	virtual void Finalize();

	///////////////////////////////////////////////////////////
	/// @brief		Implement threading
	///
	/// Run() Ends when the thread exits To continue the thread
	/// Run() Write loop processing etc. so as not to get out of
	/// Properly describe sleep and standby processing, and do not increase the system load
	/// Be careful
	///
	/// Thread::Cancel() Is called suddenly during the processing of Run()
	/// Run() Be aware that will end
	///
	///////////////////////////////////////////////////////////
	virtual void Run();

	///////////////////////////////////////////////////////////
	/// @brief		Implement the process to be executed after the thread process is completed or canceled
	///
	/// Default does nothing
	///
	///////////////////////////////////////////////////////////
	virtual void Cleanup();

protected:
	/// Thread
	Thread m_thread;

	/// Thread cycle frequency
	unsigned int m_freq;

	/// Exit thread or not
	bool m_exit;

	///////////////////////////////////////////////////////////
	/// @brief		Start thread
	/// @param[in]	_name Thread name
	/// @param[in]	_isBlock Is block
	/// @return		void
	/// @note
	///////////////////////////////////////////////////////////
	virtual void StartThread(const std::string& _name, bool _isBlock);

	///////////////////////////////////////////////////////////
	/// @brief		Stop thread
	/// @param[in]	_wait Wait thread stop
	/// @note
	///////////////////////////////////////////////////////////
	virtual void StopThread(bool _wait = true);

  	///////////////////////////////////////////////////////////
	/// @brief		On thread start
	/// @return		bool
	/// @note       If the return value is false, the thread stops
	///////////////////////////////////////////////////////////
	virtual bool OnStart() = 0;  

	///////////////////////////////////////////////////////////
	/// @brief		On thread stop
	/// @return		void
	/// @note
	///////////////////////////////////////////////////////////
	virtual void OnStop() = 0;

	///////////////////////////////////////////////////////////
	/// @brief		On thread looping
	/// @return		bool
	/// @note       If the return value is false, the thread stops
	///////////////////////////////////////////////////////////
	virtual bool OnLooping() = 0;

};
}

#endif
