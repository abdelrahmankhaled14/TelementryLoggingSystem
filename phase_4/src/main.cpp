#include "threadpool.hpp"
#include "logmanager.hpp"
#include "consolesink.hpp"
#include "filesink.hpp"
#include "formatter.hpp"
#include "policies.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "ringbuffer.hpp"

#include <iostream>
#include <mutex>
#include <condition_variable>

int main()
{ /* ---------------- THREADPOOL ---------------- */
    ThreadPool pool(3, 50);
    std::cout << "Telemetry logging app started\n";

    /* ---------------- SINKS ---------------- */
    consolesink console;
    filesink file1("log1.txt");
    filesink file2("log2.txt");

    logmanagerbuilder builder(20);
    builder.addSink(&console)
        .addSink(&file1)
        .addSink(&file2);

    LogManager logger = builder.build();

    /* ---------------- TELEMETRY SOURCE ---------------- */
    FileTelemetrySourceImpl source("telemetry.txt");
    if (!source.OpenSource())
    {
        std::cerr << "Failed to open telemetry source\n";
        return 1;
    }

    /* ---------------- SHARED STATE ---------------- */
    RingBuffer<logmessage> formattedQueue(50);
    std::mutex mtx;
    std::condition_variable cv;
    std::condition_variable cv2;
    bool consumerDone = false;
    bool done = false;

    /* ---------------- LOGGER (CONSUMER) ---------------- */
    pool.addTask([&]
                 {
        while (true)
        {
            
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] {
                return !formattedQueue.isEmpty() || done;
            });

            while (!formattedQueue.isEmpty())
            {
                auto msg = formattedQueue.tryPop();
                lock.unlock();

                if (msg)
                {
                    logger.log(*msg);
                    logger.flush();
                }

                lock.lock();
            }

            if (done && formattedQueue.isEmpty())
            {
                consumerDone = true;
                cv2.notify_all();
                break;
            }
                
        } });

    /* ---------------- FORMATTER (PRODUCER) ---------------- */
    pool.addTask([&]
                 {
        std::string raw;

        while (source.ReadSource(raw))
        {
            auto msgOpt = LogFormatter<CpuPolicy>::formatDataToLogMsg(raw);
            if (!msgOpt)
                continue;

            {
                std::lock_guard<std::mutex> lock(mtx);
                formattedQueue.tryPush(std::move(*msgOpt));
            }

            cv.notify_one();
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            done = true;
        }

        cv.notify_all(); });

    /* ---------------- WAIT FOR WORK ---------------- */
    std::unique_lock<std::mutex> lock(mtx);
    cv2.wait(lock, [&]
             { return consumerDone; });
    pool.shutdown();
    std::cout << "Telemetry logging finished\n";
    return 0;
}
