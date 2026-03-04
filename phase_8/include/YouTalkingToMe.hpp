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

class YouTalkingToMe {
public:
    explicit YouTalkingToMe(const std::string& configPath);
    void start();

private:
    AppConfig config;

    ThreadPool pool;
    RingBuffer<logmessage> formattedQueue;
    std::atomic<bool> done{false};
    std::atomic<bool> consumerDone{false};

    consolesink consoleSink;
    filesink cpuFileSink;
    filesink ramFileSink;
    filesink tempFileSink;
    LogManager logger;

    CommonAPITelemetrySourceImpl& source;

    void setupLogger();
    void runConsumer();
    void runProducer();
    void pushMeasurement(const std::string& policyName, const std::string& valueStr);
};