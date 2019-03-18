//
// Created by vladislav on 16.03.19.
//

#include "threadpool.h"

#include <cstring>
#include <functional>
#include <threadpool.h>


ThreadPool::ThreadPool(size_t threadCount) : stop(false)
{
    init(threadCount);
}

ThreadPool::ThreadPool() : stop(false)
{
    size_t threadCount = std::max((unsigned)min_threads_count, std::thread::hardware_concurrency());
    init(threadCount);
}

ThreadPool::~ThreadPool()
{
    stop = true;
    for (auto& thread : threads)
    {
        thread.join();
    }
}



void ThreadPool::worker()
{
    while(!stop)
    {
        std::function<void()> task;
        pop(&task);
        if (task)
        {
            task();
        } else {
            std::this_thread::yield();
        }
    }
}

void ThreadPool::init(size_t threadCount)
{
    try {
        for (size_t i = 0; i < threadCount; ++i)
        {
            threads.emplace_back(std::thread(&ThreadPool::worker, this));
        }
    } catch (...)
    {
        stop = true;
        throw std::runtime_error("threads create" + std::string(strerror(errno)));
    }
}

void ThreadPool::pop(std::function<void()>* task)
{
    std::unique_lock<std::mutex> lock(queueMutex);
    if (!tasks.empty())
    {
        *task = std::move(tasks.front());
        tasks.pop();
    }
}

void ThreadPool::addTask(std::function<void()> task)
{
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.push(task);
}
