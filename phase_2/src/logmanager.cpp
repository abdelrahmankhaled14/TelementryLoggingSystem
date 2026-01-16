#include "logmanager.hpp"

LogManager::LogManager(Ilogsink *sink)
{
    currentSink = sink;
}

void LogManager::log(const logmessage &msg)
{
    messageBuffer.push_back(msg);
}
void LogManager::flush()
{
    for (int i = 0; i < messageBuffer.size() ; i++)
    {
        currentSink->write(messageBuffer[i]);
    }
    messageBuffer.clear();
}
void LogManager::clear()
{
    messageBuffer.clear();
}
size_t LogManager::getMessageCount() const
{
    return messageBuffer.size();
}
bool LogManager::isEmpty() const
{
    bool state = false;
    if(messageBuffer.size() == 0)
    {
        state = true;
    }
    return state;
}