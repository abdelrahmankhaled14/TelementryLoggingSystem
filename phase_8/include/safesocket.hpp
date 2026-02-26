#pragma once

#include <string>

class SafeSocket {
private:
    int fd_;
    std::string path_;
    
public:
    explicit SafeSocket(const std::string& socketPath);
    
    ~SafeSocket();
    
    SafeSocket(const SafeSocket&) = delete;
    SafeSocket& operator=(const SafeSocket&) = delete;
    
    SafeSocket(SafeSocket&& other) noexcept;
    SafeSocket& operator=(SafeSocket&& other) noexcept;
    
    bool Connect();
    
    bool isOpen() const;
    
    ssize_t Read(char* buffer, size_t count);
        
    int getFd() const;
};

