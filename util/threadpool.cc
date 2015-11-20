//
// Created by wastl on 19.11.15.
//

#include "threadpool.h"

namespace marmotta {
namespace util {


ThreadPool::ThreadPool(int numWorkers) : running(true) {
    for (int i=0; i<numWorkers; i++) {
        workers.push_back(std::thread(&ThreadPool::Worker, this));
    }
}

ThreadPool::~ThreadPool() {
    Join();
}

void ThreadPool::Worker() {
    while (running || !taskQueue.empty()) {
        std::unique_lock<std::mutex> lock(taskQueueMutex);
        if (taskQueue.empty()) {
            taskQueueWait.wait(lock);
        }

        auto next = taskQueue.front();
        taskQueue.pop();
        lock.unlock();
        next();
    }
}

void ThreadPool::Schedule(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(taskQueueMutex);
    taskQueue.push(task);
    taskQueueWait.notify_one();
}

void ThreadPool::Stop() {
    running = false;
}

void ThreadPool::Join() {
    Stop();
    for (auto &t : workers) {
        t.join();
    }
}
}
}