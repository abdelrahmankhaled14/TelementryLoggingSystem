#ifndef COMMONAPI_TELEMETRY_SOURCE_IMPL_HPP
#define COMMONAPI_TELEMETRY_SOURCE_IMPL_HPP

#include <CommonAPI/CommonAPI.hpp>
#include <v1/logger/methods/loggingProxy.hpp>
#include "ITelemetrySource.hpp"
#include <string>
#include <memory>

class CommonAPITelemetrySourceImpl : public ITelemetrySource {
public:
    // Singleton
    static CommonAPITelemetrySourceImpl& getInstance() {
        static CommonAPITelemetrySourceImpl instance;
        return instance;
    }
    
    // Delete copy
    CommonAPITelemetrySourceImpl(const CommonAPITelemetrySourceImpl&) = delete;
    CommonAPITelemetrySourceImpl& operator=(const CommonAPITelemetrySourceImpl&) = delete;
    
    // Your interface
    bool OpenSource() override;
    bool ReadSource(std::string& out) override;
    
    ~CommonAPITelemetrySourceImpl();

private:
    CommonAPITelemetrySourceImpl();
    
    std::shared_ptr<v1::logger::methods::loggingProxy<>> proxy_;
    bool is_open_ = false;
};

#endif