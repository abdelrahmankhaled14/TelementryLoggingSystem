// YourApplicationName.hpp
#pragma once

#include "config.hpp"
#include "threadpool.hpp"
#include "logmanager.hpp"
#include "consolesink.hpp"
#include "filesink.hpp"
#include "formatter.hpp"
#include "policies.hpp"
#include "CommonAPITelemetrySourceImpl.hpp"
#include "ringbuffer.hpp"

#include <atomic>
#include <memory>

class YourApplicationName {
public:
    explicit YourApplicationName(const std::string& configPath);
    void start();

private:
    AppConfig config;

    // Core infra
    ThreadPool pool;
    RingBuffer<logmessage> formattedQueue;
    std::atomic<bool> done{false};
    std::atomic<bool> consumerDone{false};

    // Sinks
    consolesink consoleSink;
    filesink cpuFileSink;
    filesink ramFileSink;
    filesink tempFileSink;
    LogManager logger;

    // Source (for now: only CommonAPI SOME/IP, you can extend later)
    CommonAPITelemetrySourceImpl& source;

    // helpers
    void setupLogger();
    void runConsumer();
    void runProducer();
    void pushMeasurement(const std::string& policyName, const std::string& valueStr);
};