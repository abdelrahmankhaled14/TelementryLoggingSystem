#include "CommonAPITelemetrySourceImpl.hpp"
#include <iostream>
#include <CommonAPI/CommonAPI.hpp>

// Constructor
CommonAPITelemetrySourceImpl::CommonAPITelemetrySourceImpl() 
    : proxy_(nullptr), is_open_(false) {
    std::cout << "[CLIENT] Singleton instance created." << std::endl;
}

// Destructor
CommonAPITelemetrySourceImpl::~CommonAPITelemetrySourceImpl() {
    std::cout << "[CLIENT] Destructor called." << std::endl;
}

// OpenSource - Initialize connection to server
bool CommonAPITelemetrySourceImpl::OpenSource() {
    if (is_open_) {
        std::cout << "[CLIENT] Already open." << std::endl;
        return true;
    }
    
    try {
        std::cout << "[CLIENT] Opening source..." << std::endl;
        
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        
        const std::string& domain = "local";
        const std::string& instance = "logger.methods.justSendHi";
        const std::string& interface = "v1.logger.methods.logging";
        
        // Create proxy to connect to server
        proxy_ = runtime->buildProxy<v1::logger::methods::loggingProxy>(
            domain, 
            instance
        );
        
        if (!proxy_) {
            std::cerr << "[CLIENT] Failed to create proxy!" << std::endl;
            return false;
        }
        
        std::cout << "[CLIENT] Proxy created, waiting for availability..." << std::endl;
        
        // Wait for service to be available
        int timeout = 50;  // 5 seconds (50 * 100ms)
        while (!proxy_->isAvailable() && timeout > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            timeout--;
        }
        
        if (!proxy_->isAvailable()) {
            std::cerr << "[CLIENT] Service not available!" << std::endl;
            return false;
        }
        
        is_open_ = true;
        std::cout << "[CLIENT] Source opened successfully, service is available." << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[CLIENT] Exception in OpenSource: " << e.what() << std::endl;
        return false;
    }
}

// ReadSource - Send request and get response
bool CommonAPITelemetrySourceImpl::ReadSource(std::string& out) {
    if (!is_open_) {
        std::cerr << "[CLIENT] Source not open. Call OpenSource() first." << std::endl;
        return false;
    }
    
    if (!proxy_) {
        std::cerr << "[CLIENT] Proxy is null!" << std::endl;
        return false;
    }
    
    try {
        std::cout << "[CLIENT] Sending request..." << std::endl;
        
        int32_t response;
        
        // Call the method on the server
        CommonAPI::CallStatus callStatus;
        proxy_->requestData(callStatus, response);
        
        if (callStatus == CommonAPI::CallStatus::SUCCESS) {
            std::cout << "[CLIENT] Got response: " << response << "%" << std::endl;
            out = std::to_string(response);
            return true;
        } else {
            std::cerr << "[CLIENT] Call failed with status: " 
                      << static_cast<int>(callStatus) << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[CLIENT] Exception in ReadSource: " << e.what() << std::endl;
        return false;
    }
}