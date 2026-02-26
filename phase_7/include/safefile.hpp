#pragma once
#include <iostream>
#include <memory>
class safefile
{
private:
    int fd;
public:
    safefile(const std::string& path, int flags);
    ~safefile();

    safefile(const safefile&) = delete;
    safefile& operator=(const safefile&) = delete;
    
    safefile(safefile&& other) noexcept;
    safefile& operator=(safefile&& other) noexcept;

    int Read(char * buffer , int buffer_size);
    bool isOpen()const;
};
