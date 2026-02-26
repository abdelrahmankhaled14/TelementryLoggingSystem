#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <CommonAPI/CommonAPI.hpp>

// Generated headers (note the v1/v1 path and names)
#include "v1/v1/logger/methods/loggingProxy.hpp"
#include "v1/v1/logger/methods/TelemetryTypes.hpp"

class CommonAPITelemetrySourceImpl {
public:
    // -------- Singleton accessor --------
    static CommonAPITelemetrySourceImpl& getInstance();

    // Open connection to CommonAPI SOME/IP service
    bool OpenSource();

    // Read one snapshot as a single string: "cpu;ram;temp"
    bool ReadSource(std::string &out);

private:
    // -------- Singleton mechanics --------
    CommonAPITelemetrySourceImpl() = default;
    ~CommonAPITelemetrySourceImpl() = default;

    CommonAPITelemetrySourceImpl(const CommonAPITelemetrySourceImpl&) = delete;
    CommonAPITelemetrySourceImpl& operator=(const CommonAPITelemetrySourceImpl&) = delete;

    // -------- CommonAPI objects --------
    std::shared_ptr<CommonAPI::Runtime> runtime_;
    std::shared_ptr<v1::v1::logger::methods::loggingProxy<>> proxy_;

    bool ready_ = false;
    std::mutex mtx_;
};