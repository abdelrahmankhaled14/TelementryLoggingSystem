#include <CommonAPI/CommonAPI.hpp>
#include <v1/logger/methods/loggingProxy.hpp>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;
using namespace v1::logger::methods;

constexpr std::string_view DOMAIN = "local";
constexpr std::string_view INSTANCE = "logger.methods.justSendHi";
constexpr std::string_view CONNECTION = "someip";

int main(int argc, char const *argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "[Client] Starting with SomeIP binding" << std::endl;
    std::cout << "========================================\n" << std::endl;

    auto runtime = CommonAPI::Runtime::get();

    std::cout << "[Client] Building proxy..." << std::endl;
    std::cout << "[Client] Domain: " << DOMAIN << std::endl;
    std::cout << "[Client] Instance: " << INSTANCE << std::endl;
    std::cout << "[Client] Connection: " << CONNECTION << std::endl;

    auto proxy = runtime->buildProxy<loggingProxy>(
        std::string(DOMAIN),
        std::string(INSTANCE),
        std::string(CONNECTION)
    );

    std::cout << "[Client] Waiting for service..." << std::endl;
    int timeout = 100;
    while (!proxy->isAvailable() && timeout > 0) {
        std::this_thread::sleep_for(50ms);
        timeout--;
    }

    if (!proxy->isAvailable()) {
        std::cerr << "[Client] Service not available!" << std::endl;
        return 1;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "[Client] Service available! Starting requests..." << std::endl;
    std::cout << "========================================\n" << std::endl;

    int iteration = 0;
    while (true) {
        iteration++;

        CommonAPI::CallStatus callStatus;
        int32_t percentage;

        proxy->requestData(callStatus, percentage);

        if (callStatus == CommonAPI::CallStatus::SUCCESS) {
            std::cout << "[Client] Iteration " << iteration << ": Received " 
                      << percentage << "%" << std::endl;
        } else {
            std::cout << "[Client] Call failed!" << std::endl;
            break;
        }

        std::this_thread::sleep_for(1s);
    }

    return 0;
}