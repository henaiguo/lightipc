///////////////////////////////////////////////////////////
/// @file	Thread.h
/// @brief	thread
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_THREAD__
#define __LIGHT_IPC_THREAD__

#include <pthread.h>
#include <string>

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class	IRunnable
/// @brief	Threading interface
/// 
/// - Thread The class manages the thread, and the actual processing is done with IRunnable::Run()
///   IRunnable The class that implements so-called worker [Worker] class
/// - Implement this interface to implement thread processing
/// - Run() method is called at thread start
/// - Cleanup() method called on thread exit/cancel (default does nothing)
///
/// Run() Lock() Mutex when threading is started in
/// Cancel() Is called, the Mutex remains locked after that,
/// Deadlocks can occur
/// Cleanup() Assumes usage such as Unlock() Mutex
///
///////////////////////////////////////////////////////////
class IRunnable
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~IRunnable() {};

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
	virtual void Run() = 0;

	///////////////////////////////////////////////////////////
	/// @brief		Implement the process to be executed after the thread process is completed or canceled
	///
	/// Default does nothing
	///
	///////////////////////////////////////////////////////////
	virtual void Cleanup() {};
};


///////////////////////////////////////////////////////////
/// @class	Thread
/// @brief	thread
/// 
/// - Thread Is a proxy class for POSIX threads
/// - Mainly manage thread lifecycle with Start()/Cancel()/Join()
/// - IRunnable Alternatively, you can implement threading by extending this class.
///
///  [ life cycle]
///   <Start> ---> < Constructor> --> <A> ---> <Destructor> --> <End>
/// 
///         +---------<-------------------<-----------------<--------+
///         |                                                        |loop
///   <A> -----+------>----------+-------->---------+------->--------+------->
///            |              |  |               |  |             |
///            +--> Start() --+  +--> Cancel() --+  +--> Join() --+
/// 
///   - <Start> From <A> to <A>, thread processing has not started, so it is inactive (IsActive() == false)
///   - Start() Immediately after that, thread processing is started and active (IsActive() == true)
///   - Cancel() Note that the thread processing is not finished immediately after the end (IsActive() is undefined)
///   - Join() After that, the thread processing is finished and is inactive (IsActive() == false)
/// 
///  [ Thread state and control]
///   The same Thread can be threaded many times in a loop, but the control changes depending on the state of IsActive()
///   - Start() : Do nothing when active (IsActive() == true)
///   - Cancel(): Do nothing when inactive (IsActive() == false)
///   - Join()  : Do nothing when inactive (IsActive() == false)
/// 
///////////////////////////////////////////////////////////
class Thread
{
public:
	///////////////////////////////////////////////////////////
	/// @brief		Get the thread that called this method
	/// @return		Thread
	/// @note		Thread Or from within IRunnable
	/// @note		main Returns NULL when called from a thread or a thread other than Thread or IRunnable
	///////////////////////////////////////////////////////////
	static Thread *CurrentThread();

	///////////////////////////////////////////////////////////
	/// @brief		Get the thread ID of the thread that called this method
	/// @return		Thread ID
	/// @note		
	///////////////////////////////////////////////////////////
	static unsigned long CurrentThreadId(); // current thread id

	///////////////////////////////////////////////////////////
	/// @brief		Release the CPU of the thread that called this method, and yield the CPU to other threads.
	/// @note		OS Switching of thread context of
	/// 			System performance may deteriorate, so use it appropriately.
	///////////////////////////////////////////////////////////
	static void Yield();

	///////////////////////////////////////////////////////////
	/// @brief		Suspend the thread that called this method for the specified number of seconds
	/// @param[in]	sec Second
	/// @note		
	///////////////////////////////////////////////////////////
	static void Sleep(unsigned int sec);

	///////////////////////////////////////////////////////////
	/// @brief		Pause the thread that called this method for the specified millisecond (10^-3) seconds
	/// @param[in]	millisec millisecond
	/// @note		
	///////////////////////////////////////////////////////////
	static void MilliSleep(unsigned int millisec);

	///////////////////////////////////////////////////////////
	/// @brief		Pause the thread that called this method for the specified micro (10^-6) seconds
	/// @param[in]	microsec Microsecond
	/// @note		
	///////////////////////////////////////////////////////////
	static void MicroSleep(unsigned int microsec);

	///////////////////////////////////////////////////////////
	/// @brief		Pause the thread that called this method for the specified nano (10^-9) seconds
	/// @param[in]	nsec Nanosecond
	/// @note		
	///////////////////////////////////////////////////////////
	static void NanoSleep(unsigned int nsec);


	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @note		Thread that does nothing (assuming use as stack variable)
	/// @note		IRRunner and parameters can be set later with SetRunner()
	//////////////////////////////////////////////////////////
	Thread();

	///////////////////////////////////////////////////////////
	/// @brief		constructor
	/// @param[in]	runnable IRunnable interface
	/// @param[in]	param The parameter
	/// @note		IRunnable And set the parameters to get in IRunnable::Run()
	///////////////////////////////////////////////////////////
	Thread(IRunnable *runnable, void *param);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	///////////////////////////////////////////////////////////
	virtual ~Thread();

	///////////////////////////////////////////////////////////
	/// @brief		IRunnable And set parameters
	/// @param[in]	runnable IRunnable interface
	/// @param[in]	param The parameter
	/// @note		Start() To be set before calling
	///////////////////////////////////////////////////////////
	void SetRunner(IRunnable *runnable, void *param);

	///////////////////////////////////////////////////////////
	/// @brief		Get parameters
	/// 
	/// Thread The parameters passed in the constructor of IRunnable::Run()
	/// void Run() {
	///     Thread *t = Thread::CurrentThread();
	///     MyParameter *param = static_cast<MyParameter*>(t->Parameter());
	///        :
	/// Can be obtained as
	/// 
	/// @return		The parameter
	/// @note		static_cast Then get
	///////////////////////////////////////////////////////////
	void *Parameter();

	///////////////////////////////////////////////////////////
	/// @brief		Set name
	/// @param[in]	name name
	/// @note		Up to 16 characters
	///////////////////////////////////////////////////////////
	void SetName(const std::string &name);

	///////////////////////////////////////////////////////////
	/// @brief		Get name
	/// @return		name
	/// @note		The default is ""
	///////////////////////////////////////////////////////////
	std::string Name();

	///////////////////////////////////////////////////////////
	/// @brief		Get thread ID
	/// @return		Thread ID
	/// @note		Start() 0 if not
	/// @note		There is no overlap in multithreading, but when the thread exits,
	/// 			Note that it may be reused in another thread
	///////////////////////////////////////////////////////////
	unsigned long ThreadId();

	///////////////////////////////////////////////////////////
	/// @brief		Check if thread is active
	/// @retval		true Active
	/// @retval		false Inactive
	/// @note		Start() It is false before and false after Join().
	/// 			Start() From after to before Join() depends on the thread state
	///////////////////////////////////////////////////////////
	bool IsActive();

	///////////////////////////////////////////////////////////
	/// @brief		Override this method when threading is used by inheriting Thread
	/// @note		
	///////////////////////////////////////////////////////////
	virtual void Execute();

	///////////////////////////////////////////////////////////
	/// @brief		Override this method when inheriting resource release processing and using
	/// @note		
	///////////////////////////////////////////////////////////
	virtual void Cleanup();

	///////////////////////////////////////////////////////////
	/// @brief		Request to start threading
	/// @note		Return before threading starts (non block)
	/// @note		IsActive() Does nothing if is true
	///////////////////////////////////////////////////////////
	void Start();

	///////////////////////////////////////////////////////////
	/// @brief		Request cancellation of threading
	/// @note		Return before threading finishes (non block)
	/// @note		Does nothing if threading has already finished
	///////////////////////////////////////////////////////////
	void Cancel();

	///////////////////////////////////////////////////////////
	/// @brief		Wait until thread processing is finished or canceled
	/// @note		Does nothing if threading has already finished
	///////////////////////////////////////////////////////////
	void Join();

private:
	/// POSIX thread
	pthread_t m_thread;

	/// IRunnable interface
	IRunnable *m_runnable;

	/// The parameter
	void *m_parameter;

	/// Name
	std::string m_name;

	/// Thread ID
	unsigned long m_threadId;

	/// Activated state
	bool m_isActive;

	///////////////////////////////////////////////////////////
	/// @brief		Copy constructor
	/// @note		Copy prohibited
	///////////////////////////////////////////////////////////
	Thread(const Thread &src);

	///////////////////////////////////////////////////////////
	/// @brief		Assignment operator
	/// @note		Substitution prohibited
	///////////////////////////////////////////////////////////
	Thread& operator=(const Thread &src);
};
}
#endif
