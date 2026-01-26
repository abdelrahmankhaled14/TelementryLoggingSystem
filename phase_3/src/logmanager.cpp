// logmanager.cpp
#include "logmanager.hpp"

LogManager::LogManager(size_t bufferCapacity)
    : messageBuffer(bufferCapacity)
{
}

void LogManager::addSink(Ilogsink *sink)
{
    currentSink.push_back(sink);
}

void LogManager::log(const logmessage &msg)
{
    messageBuffer.tryPush(msg);
}

void LogManager::flush()
{
    while (!messageBuffer.isEmpty())
    {
        auto msgOpt = messageBuffer.tryPop();
        if (msgOpt)
        {
            for (auto &sink : currentSink)
            {
                sink->write(*msgOpt);
            }
        }
    }
}

void LogManager::clear()
{
    messageBuffer.clear();
}

size_t LogManager::getMessageCount() const
{
    return messageBuffer.getCount();
}

bool LogManager::isEmpty() const
{
    return messageBuffer.isEmpty();
}

logmanagerbuilder::logmanagerbuilder(size_t bufferCapacity) : bufferCapacity(bufferCapacity)
{
}

logmanagerbuilder &logmanagerbuilder::setBufferCapacity(size_t capacity)
{
    bufferCapacity = capacity;
    return *this;
}
logmanagerbuilder &logmanagerbuilder::addSink(Ilogsink *sink)
{
    sinks.push_back(sink);
    return *this;
}
LogManager logmanagerbuilder::build()
{
    LogManager manager(bufferCapacity);
    for (auto &sink : sinks)
    {
        manager.addSink(sink);
    }
    return manager;
}