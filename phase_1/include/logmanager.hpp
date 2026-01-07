#pragma once
#include "Ilogsink.hpp"
#include "logmessage.hpp"
#include <vector>
#include <string>

class LogManager
{
private:
    std::vector<logmessage> messageBuffer;  
    Ilogsink* currentSink;                   
    
public:
    // Constructor - takes a sink pointer
    LogManager(Ilogsink* sink);
    
    // Default constructor - no sink initially
    LogManager() = delete;
    
    // Destructor
    ~LogManager() = default;
    
    
    
    // Log a message by providing an already-created logmessage object
    void log(const logmessage& msg);
    
    // Flush all buffered messages to the CURRENT sink
    void flush();
    
    // Clear all messages from the buffer
    void clear();
    
    // Get the number of messages currently in the buffer
    size_t getMessageCount() const;
    
    // Check if the buffer is empty
    bool isEmpty() const;
    
};