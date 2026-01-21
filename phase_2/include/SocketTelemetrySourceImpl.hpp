#pragma once

#include "ITelemetrySource.hpp"
#include "safesocket.hpp"
#include <string>
#include <memory>

class SocketTelemetrySourceImpl : public ITelemetrySource {
private:
    std::string socketPath_;
    std::unique_ptr<SafeSocket> socket_;
    
public:
    explicit SocketTelemetrySourceImpl(const std::string& socketPath);
    
    // Implements ITelemetrySource interface
    bool OpenSource() override;
    bool ReadSource(std::string& out) override;
};

