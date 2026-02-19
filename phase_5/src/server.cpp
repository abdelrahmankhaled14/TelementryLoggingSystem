#include <v1/logger/methods/loggingStubDefault.hpp>
#include <CommonAPI/CommonAPI.hpp>
#include <random>
#include <string>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

constexpr std::string_view DOMAIN = "local";
constexpr std::string_view INSTANCE = "logger.methods.justSendHi";

class LoggingService : public v1::logger::methods::loggingStubDefault {
private:
    int32_t randomInt(int32_t min = 0, int32_t max = 100) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int32_t> dist(min, max);
        return dist(gen);
    }

    int32_t getLoadPercentage() {
        return randomInt();
    }

public:
    void requestData(
        const std::shared_ptr<CommonAPI::ClientId> _client,
        requestDataReply_t _reply) override {
        int32_t percentage = getLoadPercentage();
        std::cout << "Client requested data: " << percentage << "%" << std::endl;
        _reply(percentage);
    }
};

int main(int argc, char const *argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "[Server] Starting..." << std::endl;
    std::cout << "========================================\n" << std::endl;

    auto runtime = CommonAPI::Runtime::get();
    std::shared_ptr<LoggingService> myService = std::make_shared<LoggingService>();

    std::cout << "[Server] Registering service..." << std::endl;
    bool success = runtime->registerService(
        std::string(DOMAIN),
        std::string(INSTANCE),
        myService
    );

    if (!success) {
        std::cerr << "[Server] Failed to register!" << std::endl;
        return 1;
    }

    std::cout << "[Server] Service registered!" << std::endl;
    std::cout << "[Server] Waiting for requests...\n" << std::endl;

    while (true) {
        std::this_thread::sleep_for(1s);
        std::cout << "[Server] Running..." << std::endl;
    }

    return 0;
}