#pragma once

#include <future>
#include "Thread.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/Assertions.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class ThreadPool final : public eos::NoCopyableMoveable
{
private:
	using ThreadAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::SingleThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static ThreadAllocator* GetAllocator();

public:
	ThreadPool() : m_stopped(false), m_shouldStopProcessing(false)
	{
	}

	ThreadPool(uint32 _threadCount) : m_stopped(false), m_shouldStopProcessing(false)
	{
		Execute(_threadCount);
	}

	void Execute(uint32 _threadCount = 6)
	{
		m_numOfThread = _threadCount;

		nwAssertReturnVoid(m_numOfThread > 0, "At least one thread over than main thread must be requested");

		m_threads.reserve(m_numOfThread);

		for (uint32 i = 0; i < m_numOfThread; ++i)
		{
			m_threads.emplace_back([this]() { Worker(); });
		}
	}

	~ThreadPool()
	{
		{
			MutexLock lock(m_queueMutex);

			m_shouldStopProcessing = true;
		}

		// Wake up all threads and wait for them to exit
		m_poolNotifier.notify_all();

		for (auto & taskThread : m_threads)
		{
			taskThread.Join();
		}
	}

	uint32 GetNumOfThread() const { return m_numOfThread; }

	template<class Function, class... Args>
	auto AddTask(Function&& _function, Args&&... _args)->std::future<typename std::result_of<Function(Args...)>::type>
	{
		using return_type = typename std::result_of<Function(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(std::bind(std::forward<Function>(_function), std::forward<Args>(_args)...));

		std::future<return_type> res = task->get_future();
		{
			MutexLock lock(m_queueMutex);

			AssertReturnValue(!m_shouldStopProcessing && !m_stopped, res, "Attempted to add task to a stopped pool");

			m_queueTask.emplace([task]() { (*task)(); });
		}

		m_poolNotifier.notify_one();

		return res;
	}

	void ForceStop()
	{
		{
			MutexLock lock(m_queueMutex);

			m_stopped = true;
		}

		m_poolNotifier.notify_all();

		for (eos::Vector<Thread, ThreadAllocator, ThreadPool::GetAllocator>::size_type i = 0; i < m_threads.size(); ++i)
		{
			m_threads[i].Join();
		}
	}

private:
	void Worker()
	{
		while (true)
		{
			// Retrieve a task
			TaskFunction task;
			{
				MutexLock lock(m_queueMutex);

				m_poolNotifier.wait(lock, [this]() { return !m_queueTask.empty() || m_shouldStopProcessing || m_stopped; });

				// When stopped and no more tasks remain, or if an emergency stop has been requested.
				if ((m_queueTask.empty() && m_shouldStopProcessing) || m_stopped)
				{
					return;
				}

				// Retrieve next task
				task = std::move(m_queueTask.front());
				m_queueTask.pop();
			}

			// Execute task
			task();
		}
	}


private:
	typedef std::function<void()>   TaskFunction;
	typedef std::mutex              Mutex;
	typedef std::unique_lock<Mutex> MutexLock;

	eos::Vector<Thread, ThreadAllocator, GetAllocator> m_threads;
	eos::Queue<TaskFunction, ThreadAllocator, GetAllocator> m_queueTask;
	Mutex m_queueMutex;
	std::condition_variable m_poolNotifier;
	uint32 m_numOfThread;
	bool m_shouldStopProcessing;
	bool m_stopped;
};

NW_NAMESPACE_END