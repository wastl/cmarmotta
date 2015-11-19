//
// Created by wastl on 19.11.15.
//

#ifndef MARMOTTA_THREADPOOL_H
#define MARMOTTA_THREADPOOL_H

#include <functional>
#include <queue>
#include <thread>
#include <memory>
#include <condition_variable>


namespace marmotta {
namespace util {

/**
 * Simple thread pool implementation. Starts the given number of worker threads
 * and allows scheduling task executions.
 */
class ThreadPool {
 public:
    ThreadPool(int numWorkers);

    ~ThreadPool();

    void Schedule(std::function<void()> task);

    void Stop();

 private:
    bool running;

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> taskQueue;
    std::mutex taskQueueMutex;
    std::condition_variable taskQueueWait;

    void Worker();
};

}
}

#endif //MARMOTTA_THREADPOOL_H
