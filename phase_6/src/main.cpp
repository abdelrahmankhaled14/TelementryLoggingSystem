#include "threadpool.hpp"
#include "logmanager.hpp"
#include "consolesink.hpp"
#include "filesink.hpp"
#include "formatter.hpp"
#include "policies.hpp"
#include "CommonAPITelemetrySourceImpl.hpp"
#include "ringbuffer.hpp"
#include <iostream>
#include <mutex>
#include <condition_variable>

int main() {
    /* ============ THREADPOOL ============ */
    ThreadPool pool(3, 50);
    std::cout << "\n========================================" << std::endl;
    std::cout << "Telemetry logging app started" << std::endl;
    std::cout << "========================================\n" << std::endl;

    /* ============ SINKS ============ */
    consolesink console;
    filesink file1("log1.txt");
    filesink file2("log2.txt");
    logmanagerbuilder builder(20);
    builder.addSink(&console)
            .addSink(&file1)
            .addSink(&file2);
    LogManager logger = builder.build();

    /* ============ TELEMETRY SOURCE (CommonAPI Client) ============ */
    std::cout << "[CLIENT] Initializing CommonAPI telemetry source..." << std::endl;
    auto& source = CommonAPITelemetrySourceImpl::getInstance();
    
    if (!source.OpenSource()) {
        std::cerr << "[CLIENT] Failed to open telemetry source" << std::endl;
        std::cerr << "[CLIENT] Make sure server is running!" << std::endl;
        return 1;
    }
    std::cout << "[CLIENT] Telemetry source opened successfully\n" << std::endl;

    /* ============ SHARED STATE ============ */
    RingBuffer<logmessage> formattedQueue(50);
    std::mutex mtx;
    std::condition_variable cv;
    std::condition_variable cv2;
    bool consumerDone = false;
    bool done = false;

    /* ============ LOGGER (CONSUMER) ============ */
    pool.addTask([&] {
        std::cout << "[LOGGER] Consumer thread started" << std::endl;
        
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] {
                return !formattedQueue.isEmpty() || done;
            });

            while (!formattedQueue.isEmpty()) {
                auto msg = formattedQueue.tryPop();
                lock.unlock();
                
                if (msg) {
                    logger.log(*msg);
                    logger.flush();
                }
                lock.lock();
            }

            if (done && formattedQueue.isEmpty()) {
                consumerDone = true;
                std::cout << "[LOGGER] Consumer finished" << std::endl;
                cv2.notify_all();
                break;
            }
        }
    });

    /* ============ FORMATTER (PRODUCER) ============ */
    pool.addTask([&] {
        std::cout << "[FORMATTER] Producer thread started" << std::endl;
        
        int iteration = 0;
        std::string raw;
        
        while (source.ReadSource(raw)) {
            iteration++;
            std::cout << "[FORMATTER] Iteration " << iteration << ": Got value from server: " << raw << std::endl;
            
            auto msgOpt = LogFormatter<CpuPolicy>::formatDataToLogMsg(raw);
            if (!msgOpt) {
                std::cout << "[FORMATTER] Failed to format message" << std::endl;
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                formattedQueue.tryPush(std::move(*msgOpt));
            }
            cv.notify_one();
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            done = true;
        }
        std::cout << "[FORMATTER] Producer finished after " << iteration << " iterations" << std::endl;
        cv.notify_all();
    });

    /* ============ WAIT FOR WORK ============ */
    std::unique_lock<std::mutex> lock(mtx);
    cv2.wait(lock, [&] { return consumerDone; });
    
    pool.shutdown();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Telemetry logging finished" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}