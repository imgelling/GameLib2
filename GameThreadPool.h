#if !defined(GAMETHREADPOOL_H)
#define GAMETHREADPOOL_H

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <concurrent_queue.h>

namespace game
{
    class ThreadPool 
    {
    public:
        // Starts the thread pool. If threadsWanted
        // is zero or negative, it will create 
        // std::thread::hardware_concurrency() threads
        // else threadsWanted threads will be created.
        void Start(const uint32_t threadsWanted);
        // Queue up work for the threads
        void Queue(const std::function<void()>& work);
        // Tells all threads to stop getting new work.
        // Threads will finish whatever work they were doing
        // before stopping.
        void Stop();
        // Is there work in the queue.  Does
        // not signify all work on threads is done.
        bool hasWork();
        // Number of worker threads total
        uint32_t NumberOfThreads() const noexcept { return (uint32_t)_threadPool.size(); }
        ~ThreadPool()
        {
            Stop();
        }

    private:
        void _ThreadLoop();

        std::mutex _terminateMutex;                     // Used when terminating the thread pool
        std::condition_variable _terminateCondition;    // Allows threads to wait on new work or termination 
        std::vector<std::thread> _threadPool;           // Threads for work
        concurrency::concurrent_queue<std::function<void()>> _work;  // List of work to do, lockless
        bool _hasStarted = false;                       // Have we started the thread pool?
        std::atomic_bool _shouldTerminate = false;      // Tells threads to stop looking for jobs
    };


    inline void ThreadPool::Start(const uint32_t threadsWanted = 0)
    {
        if (_hasStarted) return;
        uint32_t numThreads = 0;
        if (threadsWanted < 1)
        {
            numThreads = std::thread::hardware_concurrency(); // Max # of threads the system supports
        }
        else
        {
            numThreads = threadsWanted;
        }
        for (uint32_t i = 0; i < numThreads; i++)
        {
            _threadPool.emplace_back(std::thread(&ThreadPool::_ThreadLoop, this));
        }
        _hasStarted = true;
        _shouldTerminate = false;
    }

	inline void ThreadPool::_ThreadLoop() 
    {
		while (true) 
        {
			std::function<void()> work;
			{
				std::unique_lock<std::mutex> lock(_terminateMutex);
				_terminateCondition.wait(lock, [this] {
					return !_work.empty() || _shouldTerminate;
					});
			}
			if (_shouldTerminate) 
            {
				return;
			}
			if (_work.try_pop(work))
			{
				work();
			}
		}
	}

    inline void ThreadPool::Queue(const std::function<void()>& work) 
    {
        if (!_hasStarted)
        {
            return;
        }
        _work.push(work);
        _terminateCondition.notify_one();
    }

    inline bool ThreadPool::hasWork() 
    {
        return !_work.empty();
    }

    inline void ThreadPool::Stop() 
    {
        if (!_hasStarted) return;
        _shouldTerminate = true;
        _terminateCondition.notify_all();
        
        for (std::thread& active_thread : _threadPool) 
        {
            if (active_thread.joinable())
            {
                active_thread.join();
            }
        }
        _threadPool.clear();
        _hasStarted = false;
    }
}

/*
EXAMPLE USAGE

#include <iostream>

#include "GameThreadPool.h"

class Test
{
public:
    void print(uint8_t letter)
    {
        //std::unique_lock<std::mutex> lock(cout_mutex);
        std::cout << letter << "dfd\n";
    }
};

void test(int i)
{
    //std::unique_lock<std::mutex> lock(cout_mutex);
    std::cout << i << "\n";
}

void test2(int i, int b)
{
    //std::unique_lock<std::mutex> lock(cout_mutex);
    std::cout << i - b << "\n";
}

void test3(int& i, int b)
{
    i++;
}

std::mutex vector;
void test4(std::vector<int>& in)
{
    {
        std::unique_lock<std::mutex> lock(vector);

        in.emplace_back(1);
    }
}


int main() {
    game::ThreadPool tp;
    Test testclass;
    std::vector<int> testVector;
    tp.Start();
    int t = 0;
    for (int i = 0; i < 100; i++)
    {
        // queue with 1 argument
        tp.Queue(std::bind(test, i));

        // queue with multiple arguments
        tp.Queue(std::bind(test2, i, i - 1));

        // queue with a reference
        tp.Queue(std::bind(test3, std::ref(t), 13));

        // queue a class method
        //auto bob = std::bind(&Test::print, &testclass, 'a' + i);
        // tp.QueueJob(bob); // or
        tp.Queue(std::bind(&Test::print, &testclass, 'a' + i));

        // queue a std:vector as a reference
        tp.Queue(std::bind(test4, std::ref(testVector)));

    }

    while (tp.hasWork())
    {
    }
    std::cout << "The number is = " << t << "\n";
    std::cout << "the vector has " << testVector.size() << " size.\n";

    tp.Stop();

    return 0;
}
*/

#endif

