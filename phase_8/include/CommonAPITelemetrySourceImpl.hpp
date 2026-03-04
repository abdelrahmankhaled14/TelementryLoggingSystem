#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <CommonAPI/CommonAPI.hpp>
#include "ITelemetrySource.hpp"

#include "v1/v1/logger/methods/loggingProxy.hpp"
#include "v1/v1/logger/methods/TelemetryTypes.hpp"

class CommonAPITelemetrySourceImpl {
public:
    static CommonAPITelemetrySourceImpl& getInstance();

    bool OpenSource();

    bool ReadSource(std::string &out);

private:
    CommonAPITelemetrySourceImpl() = default;
    ~CommonAPITelemetrySourceImpl() = default;

    CommonAPITelemetrySourceImpl(const CommonAPITelemetrySourceImpl&) = delete;
    CommonAPITelemetrySourceImpl& operator=(const CommonAPITelemetrySourceImpl&) = delete;

    std::shared_ptr<CommonAPI::Runtime> runtime_;
    std::shared_ptr<v1::v1::logger::methods::loggingProxy<>> proxy_;

    bool ready_ = false;
    std::mutex mtx_;
};