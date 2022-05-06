#include <stdio.h>
#include <unistd.h>
#include <queue>

#include "ThreadAbstract.h"
#include "MutexLock.h"

using namespace LightIPC;

class TestThread : public ThreadAbstract
{
public:
    TestThread()
    {
        ThreadAbstract::Initialize();
    }

    void Start(std::string _name, bool _isBlock)
    {
        ThreadAbstract::StartThread(_name, _isBlock);
    }

    void Stop()
    {
        ThreadAbstract::StopThread();
    }

    void SetSum(int _sum)
    {
        MutexLock lock(&m_mutex);
        m_queue.push(_sum);
    }

	bool OnStart()
    {
        ::printf("%s: OnStart\n", m_thread.Name().c_str());
    }

	void OnStop()
    {
        ::printf("%s: OnStop\n", m_thread.Name().c_str());
    }

	bool OnLooping()
    {
        MutexLock lock(&m_mutex);
        if (m_queue.empty()) return true;

        int sum = m_queue.front();
        m_queue.pop();
        ::printf("%s: OnLooping[%d]\n", m_thread.Name().c_str(), sum); 

        // When sum is greater than 5, end the thread
        return sum <= 5;
    }

private:
    std::queue<int> m_queue;
    Mutex m_mutex;
};

void test0()
{
	printf("\ntest0()\n");
    TestThread test;
    test.Start("test0", false);

    int sum = 0;
    while (sum < 10) {
        test.SetSum(sum);
        Thread::Sleep(1);
    }    
}

void test1()
{
	printf("\ntest1()\n");
    TestThread test;
    test.Start("test1", false);

    int sum = 0;
    while (sum < 3) {
        test.SetSum(sum);
        Thread::Sleep(1);
    }    
}

int main(int argc, char *argv[]) {
	test0();
	test1();
	return 0;
}