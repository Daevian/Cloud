#include "ThreadPool.h"
#include "../utilities/DefinesAsserts.h"
#include "../utilities/DefinesMacros.h"
//#include "../debugging/DebugLog.h"


Cloud::ThreadPool::ThreadPool(uint32 numThreads)
{
    for (size_t i = 0; i < numThreads; i++)
    {
        m_threads.push_back(std::thread(&ThreadPool::ExecuteTask, this));
    }
}

Cloud::ThreadPool::~ThreadPool()
{
    m_run = false;

    m_cv.notify_all();
    for (auto& thread : m_threads)
    {
        thread.join();
    }

    CL_ASSERT_NULL(!m_taskQueue.size());
}

void Cloud::ThreadPool::ExecuteTask()
{
    uint32 spinCounter = 0;
    while (m_run)
    {
        std::function<void()> task;
        bool runTask = false;
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            if (m_taskQueue.size() > 0)
            {
                task = m_taskQueue[0];
                m_taskQueue.erase(m_taskQueue.begin());
                runTask = true;
            }

            if (!runTask)
            {
                m_cv.wait(lock);
            }
        }

        if (runTask)
        {
            spinCounter = 0;
            task();
        }
        else
        {
            // better response time than cv, but more cpu usage

            //spinCounter++;
            //if (spinCounter >= 10)
            //{
            //    spinCounter = 0;
            //    std::this_thread::yield();
            //}
        }
    }
}


Cloud::CoreThreadPools* Cloud::CoreThreadPools::s_instance = nullptr;

Cloud::CoreThreadPools::CoreThreadPools()
{

}

Cloud::CoreThreadPools::~CoreThreadPools()
{

}

bool Cloud::CoreThreadPools::Create()
{
    CL_ASSERT(s_instance == 0, "CoreThreadPools already created. Can't re-create!");

    if (s_instance == 0)
    {
        s_instance = new CoreThreadPools();
        return s_instance->Initialise();
    }

    return false;
}

void Cloud::CoreThreadPools::Destroy()
{
    CL_ASSERT(s_instance != 0, "CoreThreadPools not created yet. Can't destroy!");

    if (s_instance != 0)
    {
        s_instance->Shutdown();
        SAFE_DELETE(s_instance);
    }
}

bool Cloud::CoreThreadPools::Initialise()
{
    uint32 numHwThreadsHint = std::thread::hardware_concurrency();
    //CL_TRACE_CHANNEL("INIT", "[CoreThreadPools] num hardware threads: %u", numHwThreadsHint);

    m_shortTaskPool = std::make_unique<ThreadPool>(numHwThreadsHint);
    return true;
}

void Cloud::CoreThreadPools::Shutdown()
{
}