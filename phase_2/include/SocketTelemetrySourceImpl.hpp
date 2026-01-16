#pragma once
#include "ITelemetrySource.hpp"

class SocketTelemetrySourceImpl : public ITelemetrySource
{
    private:
    
    public:
    SocketTelemetrySourceImpl();
    bool OpenSource() override;
    bool ReadSource(std::string &out)override;
    ~SocketTelemetrySourceImpl();
};