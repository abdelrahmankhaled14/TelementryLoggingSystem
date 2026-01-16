#include "FileTelemetrySourceImpl.hpp"
#include <fcntl.h>

FileTelemetrySourceImpl::FileTelemetrySourceImpl(const std::string& path)
    : path_(path), file_(nullptr) {}

bool FileTelemetrySourceImpl::OpenSource() {
    file_ = std::make_unique<safefile>(path_, O_RDONLY);
    return file_->isOpen();
}

bool FileTelemetrySourceImpl::ReadSource(std::string& out) {
    if (!file_) return false;
    
    char buffer[4096];
    ssize_t bytes = file_->Read(buffer, sizeof(buffer));
    if (bytes <= 0) return false;
    
    out.assign(buffer, bytes);
    return true;
}
