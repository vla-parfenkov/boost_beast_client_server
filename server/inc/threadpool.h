//
// Created by vladislav on 16.03.19.
//

#ifndef SAMPLE_SERVER_THREADPOOL_H
#define SAMPLE_SERVER_THREADPOOL_H

#include <thread>
#include <queue>
#include <mutex>
#include <map>
#include <atomic>
#include <functional>

class ThreadPool {

private:

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::mutex mapMutex;
    std::atomic_bool stop;

    static const size_t min_threads_count = 2;

    void worker();
    void init(size_t threadCount);
    void pop(std::function<void()>* task);


public:

    ThreadPool(size_t threadCount);
    ThreadPool();
    ~ThreadPool();

    void addTask(std::function<void()> task);
};


#endif //SAMPLE_SERVER_THREADPOOL_H
