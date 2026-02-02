#include "FileTelemetrySourceImpl.hpp"
#include <fcntl.h>

FileTelemetrySourceImpl::FileTelemetrySourceImpl(const std::string& path)
    : path_(path), file_(nullptr) {}

bool FileTelemetrySourceImpl::OpenSource()
 {
    file_ = std::make_unique<safefile>(path_, O_RDONLY);
    return file_->isOpen();
}


bool FileTelemetrySourceImpl::ReadSource(std::string& out)  {
        if (!file_ || !file_->isOpen()) return false;

        char buffer[128]{};
        int bytesRead = file_->Read(buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0 && leftover_.empty()) return false;

        buffer[bytesRead] = '\0';
        leftover_ += buffer;

        size_t pos = leftover_.find('\n');
        if (pos != std::string::npos) {
            out = leftover_.substr(0, pos);
            leftover_ = leftover_.substr(pos + 1);
            return true;
        }

        if (!leftover_.empty()) {
            out = leftover_;
            leftover_.clear();
            return true;
        }

        return false;
    }