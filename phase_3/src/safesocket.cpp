#include "safesocket.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

SafeSocket::SafeSocket(const std::string &socketPath)
    : fd_(-1), path_(socketPath)
{
}

SafeSocket::~SafeSocket()
{
    if (fd_ != -1)
    {
        close(fd_);
    }
}

SafeSocket::SafeSocket(SafeSocket &&other) noexcept
    : fd_(other.fd_), path_(std::move(other.path_))
{
    other.fd_ = -1;
}

SafeSocket &SafeSocket::operator=(SafeSocket &&other) noexcept
{
    /*
    TODO: Close current fd_ if valid
    TODO: Steal from other
    TODO: Invalidate other
    TODO: Return *this
    */
    if (this != &other)
    {
        if (fd_ != -1)
        {
            close(fd_);
        }
        fd_ = other.fd_;
        path_ = std::move(other.path_);
        other.fd_ = -1;
        return *this;
    }
}

bool SafeSocket::Connect()
{
    // TODO: 1. Create socket: socket(AF_UNIX, SOCK_STREAM, 0)
    // TODO: 2. Setup sockaddr_un structure with path_
    // TODO: 3. Call ::connect()
    // TODO: 4. Return success/failure
    fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_ == -1)
    {
        return false;
    }
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, path_.c_str(), sizeof(server_addr.sun_path) - 1);

    int result = connect(fd_, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result == -1)
    {
        close(fd_);
        fd_ = -1;
        return false;
    }
    return true;
}

bool SafeSocket::isOpen() const
{
    return fd_ != -1;
}

ssize_t SafeSocket::Read(char *buffer, size_t count)
{
    ssize_t readbytes = read(fd_, buffer, count);
    return readbytes;
}

int SafeSocket::getFd() const
{
    return fd_;
}