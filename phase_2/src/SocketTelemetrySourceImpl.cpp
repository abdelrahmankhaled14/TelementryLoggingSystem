#include "SocketTelemetrySourceImpl.hpp"

SocketTelemetrySourceImpl::SocketTelemetrySourceImpl(const std::string &socketPath)
    : socketPath_(socketPath), socket_(nullptr)
{
}

bool SocketTelemetrySourceImpl::OpenSource()
{
    socket_ = std::make_unique<SafeSocket>(socketPath_);
    return socket_->isOpen();
}

bool SocketTelemetrySourceImpl::ReadSource(std::string &out)
{
    /*
    TODO: 1. Check if socket_ exists and is open
    TODO: 2. Create buffer (e.g., char buffer[4096])
    TODO: 3. Call socket_->read(buffer, size)
    TODO: 4. Assign to out with proper byte count
    TODO: 5. Return success/failure
    */
    if (socket_)
    {
        char buffer[4096];
        socket_->Read(buffer, 4096);

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