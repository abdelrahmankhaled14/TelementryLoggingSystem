#include "YouTalkingToMe.hpp"
#include "config.hpp"

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

YouTalkingToMe::YouTalkingToMe(const std::string& configPath)
    : config(loadConfig(configPath)),                
      pool(3, 200),                               
      formattedQueue(200),                           
      cpuFileSink("cpu.log"),                       
      ramFileSink("ram.log"),
      tempFileSink("temp.log"),
      logger(100),                                  
      source(CommonAPITelemetrySourceImpl::getInstance())
{
    std::cout << "\n========================================\n"
              << "Telemetry logging app created (config: " << configPath << ")\n"
              << "========================================\n\n";

    setupLogger();                                  
}

void YouTalkingToMe::setupLogger()
{
    logmanagerbuilder builder(100);

    if (config.sinkConsole) {
        builder.addSink(&consoleSink);
    }
    if (config.sinkCpuFile) {
        builder.addSink(&cpuFileSink);
    }
    if (config.sinkRamFile) {
        builder.addSink(&ramFileSink);
    }
    if (config.sinkTempFile) {
        builder.addSink(&tempFileSink);
    }

    logger = builder.build();
}

void YouTalkingToMe::pushMeasurement(const std::string& policyName,
                                          const std::string& valueStr)
{
    
    std::optional<logmessage> msg;

    if (policyName == "cpu") {
        msg = LogFormatter<CpuPolicy>::formatDataToLogMsg(valueStr);
    } else if (policyName == "ram") {
        msg = LogFormatter<RamPolicy>::formatDataToLogMsg(valueStr);
    } else if (policyName == "temp") {
        msg = LogFormatter<TempPolicy>::formatDataToLogMsg(valueStr);
    } else {
        
        return;
    }

    if (!msg) {
        return;
    }

    while (!formattedQueue.tryPush(std::move(*msg))) {
        std::this_thread::yield();
    }
}

void YouTalkingToMe::runConsumer()
{
    std::cout << "[LOGGER] Consumer thread started\n";

    constexpr int  FLUSH_EVERY   = 10;
    constexpr auto SPIN_SLEEP_US = 200;
    int msgCount = 0;

    while (true) {
        auto msg = formattedQueue.tryPop();

        if (msg) {
            logger.log(*msg);
            ++msgCount;

            if (msgCount % FLUSH_EVERY == 0) {
                logger.flush();
            }
        } else {
            if (done.load(std::memory_order_acquire)) {
                while (auto last = formattedQueue.tryPop()) {
                    logger.log(*last);
                }
                logger.flush();
                break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(SPIN_SLEEP_US));
        }
    }

    std::cout << "[LOGGER] Consumer finished — " << msgCount << " messages logged\n";
    consumerDone.store(true, std::memory_order_release);
}

void YouTalkingToMe::runProducer()
{
    std::cout << "[FORMATTER] Producer thread started\n";

    int iteration = 0;
    std::string raw;


    std::cout << "[CLIENT] Initializing telemetry source (" << config.sourceType << ")...\n";
    if (!source.OpenSource()) {
        std::cerr << "[CLIENT] Failed to open telemetry source\n"
                  << "[CLIENT] Make sure server is running!\n";
        done.store(true, std::memory_order_release);
        return;
    }
    std::cout << "[CLIENT] Telemetry source opened successfully\n\n";

    while (source.ReadSource(raw)) {
        ++iteration;
        std::cout << "[FORMATTER] Iteration " << iteration
                  << ": " << raw << "\n";

        if (config.sourceType == "someip") {
            int v0 = 0, v1 = 0, v2 = 0;
            char s1 = 0, s2 = 0;
            {
                std::stringstream ss(raw);
                if (!(ss >> v0 >> s1 >> v1 >> s2 >> v2) ||
                    s1 != ';' || s2 != ';') {
                    std::cout << "[FORMATTER] Parse error: " << raw << "\n";
                    continue;
                }
            }

            int values[3] = {v0, v1, v2};

            
            if (config.mapping.size() == 3) {
                for (int i = 0; i < 3; ++i) {
                    pushMeasurement(config.mapping[i],
                                    std::to_string(values[i]));
                }
            } else {
               
                pushMeasurement("cpu",  std::to_string(v0));
                pushMeasurement("temp", std::to_string(v1));
                pushMeasurement("ram",  std::to_string(v2));
            }
        } else {
           
            int value = 0;
            {
                std::stringstream ss(raw);
                if (!(ss >> value)) {
                    std::cout << "[FORMATTER] Parse error (single value): " << raw << "\n";
                    continue;
                }
            }
            pushMeasurement(config.policy, std::to_string(value));
        }

        if (config.parseMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.parseMs));
        }
    }

    done.store(true, std::memory_order_release);
    std::cout << "[FORMATTER] Producer finished after "
              << iteration << " iterations\n";
}

void YouTalkingToMe::start()
{
    std::cout << "\n========================================\n"
              << "Telemetry logging app started\n"
              << "========================================\n\n";

    pool.addTask([this] {
        runConsumer();
    });

    pool.addTask([this] {
        runProducer();
    });

    while (!consumerDone.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    pool.shutdown();

    std::cout << "\n========================================\n"
              << "Telemetry logging finished\n"
              << "========================================\n\n";
}