#include "safefile.hpp"
#include <unistd.h>
#include <fcntl.h>
safefile::safefile(const std::string &path, int flags)
{
    fd = open(path.c_str(), flags);
}
safefile::~safefile()
{
    if (fd != -1)
    {
        close(fd);
    }
}
safefile::safefile(safefile &&other) noexcept
{
    fd = other.fd;
    other.fd = -1;
}
safefile &safefile::operator=(safefile &&other) noexcept
{
    if (this != &other)
    {
        if (fd != -1)
        {
            close(fd);
        }
        fd = other.fd;
        other.fd = -1;
    }
    return *this;
}
int safefile::Read(char *buffer, int buffer_size)
{
    return read(fd, buffer, buffer_size);
}
bool safefile::isOpen ()const
{
    if (fd == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}