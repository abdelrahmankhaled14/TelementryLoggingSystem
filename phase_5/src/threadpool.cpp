#include "threadpool.hpp"
#include "iostream"

ThreadPool::ThreadPool(size_t threadsize, size_t tasksize) : _taskQueue(tasksize), _threadQueue(threadsize), _shutdown(false)
{
    for (int i = 0; i < _threadQueue.getCapacity(); i++)
    {
        _threadQueue.tryPush(std::thread([this]()
                                         { this->loop(); }));
    }
}

ThreadPool::~ThreadPool()
{
    while (!_threadQueue.isEmpty())
    {
        auto optThread = _threadQueue.tryPop();
        if (optThread && optThread->joinable())
        {
            optThread->join();
        }
    }
}

ThreadPool::ThreadPool(ThreadPool &&other)
{
    this->_taskQueue = std::move(other._taskQueue);
    this->_threadQueue = std::move(other._threadQueue);
}

ThreadPool &ThreadPool::operator=(ThreadPool &&other)
{
    if (&other != this)
    {
        this->_taskQueue = std::move(other._taskQueue);
        this->_threadQueue = std::move(other._threadQueue);
    }
    return *this;
}

bool ThreadPool::addTask(std::function<void(void)> obj)
{

    std::lock_guard<std::mutex> lock(_mtx);
    _taskQueue.tryPush(obj);
    _cv.notify_one();
    return true;
}

void ThreadPool::loop(void)
{
    while (true)
    {
        std::function<void()> task;
        std::unique_lock lock(_mtx);
        _cv.wait(lock, [this]()
                 { return !_taskQueue.isEmpty() || _shutdown; });
        if (_shutdown && _taskQueue.isEmpty())
        {
            break;
        }
        auto opttask = _taskQueue.tryPop();
        if (opttask)
        {
            task = std::move(opttask.value());
        }
        lock.unlock();
        if (task)
        {
            task();
        }
    }
}
void ThreadPool::shutdown(void)
{
    _shutdown = true;
    _cv.notify_all();
}