// logmanager.hpp
#pragma once
#include "Ilogsink.hpp"
#include "logmessage.hpp"
#include "ringbuffer.hpp"
#include <vector>

class LogManager
{
private:
    RingBuffer<logmessage> messageBuffer;
    std::vector <Ilogsink*> currentSink;

public:
    LogManager( size_t bufferCapacity = 10);
    
    LogManager() = delete;
    
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    
    LogManager(LogManager&&) = default;
    LogManager& operator=(LogManager&&) = default;
    
    ~LogManager() = default;

    void addSink(Ilogsink* sink);
   

    void log(const logmessage& msg);

    void flush();

    void clear();

    size_t getMessageCount() const;

    bool isEmpty() const;
};

class logmanagerbuilder
{
private:
    size_t bufferCapacity;
    std::vector<Ilogsink*> sinks;
public:
    logmanagerbuilder(size_t bufferCapacity );
    logmanagerbuilder& setBufferCapacity(size_t capacity);

    logmanagerbuilder& addSink(Ilogsink* sink);

    LogManager build();
};