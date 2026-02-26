#include "threadpool.hpp"
#include "logmanager.hpp"
#include "consolesink.hpp"
#include "filesink.hpp"
#include "formatter.hpp"
#include "policies.hpp"
#include "CommonAPITelemetrySourceImpl.hpp"
#include "ringbuffer.hpp"

#include <iostream>
#include <atomic>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>

int main() {
    /* ============ THREADPOOL ============ */
    ThreadPool pool(3, 200);
    std::cout << "\n========================================\n"
              << "Telemetry logging app started\n"
              << "========================================\n\n";

    /* ============ SINKS ============ */
    consolesink console;
    filesink cpuFile("cpu.log");
    filesink ramFile("ram.log");
    filesink tempFile("temp.log");

    logmanagerbuilder builder(100);   // larger internal queue
    builder.addSink(&console)
           .addSink(&cpuFile)
           .addSink(&ramFile)
           .addSink(&tempFile);

    LogManager logger = builder.build();

    /* ============ TELEMETRY SOURCE ============ */
    std::cout << "[CLIENT] Initializing CommonAPI telemetry source...\n";
    auto &source = CommonAPITelemetrySourceImpl::getInstance();

    if (!source.OpenSource()) {
        std::cerr << "[CLIENT] Failed to open telemetry source\n"
                  << "[CLIENT] Make sure server is running!\n";
        return 1;
    }
    std::cout << "[CLIENT] Telemetry source opened successfully\n\n";

    /* ============ SHARED STATE ============
     *  Lock-free flag + large ring so producer never blocks.
     *  Consumer spins with a tiny sleep to avoid burning a core.
     */
    RingBuffer<logmessage> formattedQueue(200);
    std::atomic<bool> done{false};
    std::atomic<bool> consumerDone{false};

    /* ============ LOGGER / CONSUMER ============ */
    pool.addTask([&] {
        std::cout << "[LOGGER] Consumer thread started\n";

        constexpr int  FLUSH_EVERY   = 10;   // flush every N messages
        constexpr auto SPIN_SLEEP_US = 200;  // µs to sleep when queue empty
        int msgCount = 0;

        while (true) {
            auto msg = formattedQueue.tryPop();

            if (msg) {
                logger.log(*msg);
                ++msgCount;

                // batch flush — avoid per-message flush cost
                if (msgCount % FLUSH_EVERY == 0) {
                    logger.flush();
                }
            } else {
                if (done.load(std::memory_order_acquire)) {
                    // drain any remaining messages
                    while (auto last = formattedQueue.tryPop()) {
                        logger.log(*last);
                    }
                    logger.flush();
                    break;
                }
                // spin-wait with minimal sleep
                std::this_thread::sleep_for(std::chrono::microseconds(SPIN_SLEEP_US));
            }
        }

        std::cout << "[LOGGER] Consumer finished — " << msgCount << " messages logged\n";
        consumerDone.store(true, std::memory_order_release);
    });

    /* ============ FORMATTER / PRODUCER ============ */
    pool.addTask([&] {
        std::cout << "[FORMATTER] Producer thread started\n";

        int iteration = 0;
        std::string raw;

        while (source.ReadSource(raw)) {
            ++iteration;
            std::cout << "[FORMATTER] Iteration " << iteration
                      << ": " << raw << "\n";

            int cpuPercent = 0, tempC = 0, ramPercent = 0;
            {
                std::stringstream ss(raw);
                char s1 = 0, s2 = 0;
                if (!(ss >> cpuPercent >> s1 >> tempC >> s2 >> ramPercent)
                    || s1 != ';' || s2 != ';') {
                    std::cout << "[FORMATTER] Parse error: " << raw << "\n";
                    continue;
                }
            }

            // ── CPU ──
            if (auto m = LogFormatter<CpuPolicy>::formatDataToLogMsg(std::to_string(cpuPercent))) {
                while (!formattedQueue.tryPush(std::move(*m)))
                    std::this_thread::yield();  // retry instead of dropping
            }

            // ── RAM ──
            if (auto m = LogFormatter<RamPolicy>::formatDataToLogMsg(std::to_string(ramPercent))) {
                while (!formattedQueue.tryPush(std::move(*m)))
                    std::this_thread::yield();
            }

            // ── TEMP ──
            if (auto m = LogFormatter<TempPolicy>::formatDataToLogMsg(std::to_string(tempC))) {
                while (!formattedQueue.tryPush(std::move(*m)))
                    std::this_thread::yield();
            }

            // NO sleep_for — run as fast as the server delivers data
        }

        done.store(true, std::memory_order_release);
        std::cout << "[FORMATTER] Producer finished after " << iteration << " iterations\n";
    });

    /* ============ WAIT ============ */
    while (!consumerDone.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    pool.shutdown();

    std::cout << "\n========================================\n"
              << "Telemetry logging finished\n"
              << "========================================\n\n";
    return 0;
}