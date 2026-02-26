#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP
#include <ringbuffer.hpp>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <optional>
#include <utility>

class ThreadPool
{
private:
    RingBuffer<std::function<void(void)>> _taskQueue;
    RingBuffer<std::thread> _threadQueue;
    std::mutex _mtx;
    std::condition_variable _cv;
    bool _shutdown;

public:
    ThreadPool(size_t threadsize  , size_t tasksize );
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool &operator =(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&);
    ThreadPool& operator=(ThreadPool&&);
    bool addTask(std::function<void(void)> obj);
    void loop(void);
    void shutdown (void);
};

#endif
