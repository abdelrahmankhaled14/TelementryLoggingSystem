#include "SocketTelemetrySourceImpl.hpp"

SocketTelemetrySourceImpl::SocketTelemetrySourceImpl(const std::string &socketPath)
    : socketPath_(socketPath), socket_(nullptr)
{
}

bool SocketTelemetrySourceImpl::OpenSource()
{
    socket_ = std::make_unique<SafeSocket>(socketPath_);
    if(socket_->Connect())
    {
        return socket_->isOpen();
    }
    return false;
}

bool SocketTelemetrySourceImpl::ReadSource(std::string &out)
{
    if (socket_ && socket_->isOpen())
    {
        char buffer[4096];
        ssize_t bytes = socket_->Read(buffer, sizeof(buffer));
        if (bytes <= 0)
        {
            return false;
        }
        out.assign(buffer, bytes);
        return true;
    }
    return false;
}