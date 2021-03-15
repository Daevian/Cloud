#pragma once

#include "DynamicArray.h"
#include <memory>
#include <functional>
#include <thread>
#include <mutex>

namespace Cloud
{
	class ThreadPool
	{
	public:
		ThreadPool(uint32 numThreads);
		~ThreadPool();

		template <typename T_Function>
		void Submit(T_Function&& task)
		{
			{
				std::unique_lock<std::mutex> lock(m_mtx);
				m_taskQueue.push_back(std::function<void()>(task));
			}

			m_cv.notify_one();
		}

	private:
		void ExecuteTask();

		Cloud::Vector<std::thread> m_threads;
		Cloud::Vector<std::function<void()>> m_taskQueue;
		std::atomic<bool> m_run = true;
		std::mutex m_mtx;
		std::condition_variable m_cv;
	};

	class CoreThreadPools
	{
	public:
		static bool Create();
		static void Destroy();

		static CoreThreadPools& Instance() { return *s_instance; }

		static ThreadPool& GetShortTaskPool() { return *Instance().m_shortTaskPool; }

	private:
		static CoreThreadPools* s_instance;

		CoreThreadPools();
		~CoreThreadPools();

		bool Initialise();
		void Shutdown();

		std::unique_ptr<ThreadPool> m_shortTaskPool;
	};
}