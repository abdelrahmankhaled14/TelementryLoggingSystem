#include "CommonAPITelemetrySourceImpl.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace v1::v1::logger::methods;


CommonAPITelemetrySourceImpl& CommonAPITelemetrySourceImpl::getInstance() {
    static CommonAPITelemetrySourceImpl instance;
    return instance;
}


bool CommonAPITelemetrySourceImpl::OpenSource() {
    std::lock_guard<std::mutex> lock(mtx_);

    if (ready_) {
        std::cout << "[CLIENT] Proxy already initialized\n";
        return true;
    }

    runtime_ = CommonAPI::Runtime::get();
    if (!runtime_) {
        std::cerr << "[CLIENT] Failed to get CommonAPI runtime!\n";
        return false;
    }

    proxy_ = runtime_->buildProxy<loggingProxy>(
        "local",
        "logger.methods.justSendHi"
    );

    std::cout << "[CLIENT] Waiting for server to become available...\n";
    while (!proxy_->isAvailable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[CLIENT] Proxy connected!\n";
    ready_ = true;
    return true;
}


bool CommonAPITelemetrySourceImpl::ReadSource(std::string &out) {
    if (!ready_) {
        std::cerr << "[CLIENT] Source not opened yet!\n";
        return false;
    }

    TelemetryTypes::TelemetrySnapshot snap;
    CommonAPI::CallStatus callStatus;

    proxy_->requestData(callStatus, snap);

    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "[CLIENT] requestData() failed, status = "
                  << static_cast<int>(callStatus) << "\n";
        return false;
    }

    const std::string totalCpuStr = snap.getCoreLoads();       
    const uint32_t ramPercent     = snap.getRamUsagePercentage();
    const uint16_t tempC          = snap.getTemperatureC();

    out = totalCpuStr + ";" +
          std::to_string(ramPercent) + ";" +
          std::to_string(tempC);

    return true;
}