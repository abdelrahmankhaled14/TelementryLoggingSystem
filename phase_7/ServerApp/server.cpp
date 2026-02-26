#include <CommonAPI/CommonAPI.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "../common-api/src-gen/v1/v1/logger/methods/loggingStubDefault.hpp"
#include "../common-api/src-gen/v1/v1/logger/methods/TelemetryTypes.hpp"
#include "telemetry_sampler.hpp"



class TelemetryLoggingStub
    : public v1::v1::logger::methods::loggingStubDefault {
public:
    TelemetryLoggingStub() = default;

    virtual void requestData(
        const std::shared_ptr<CommonAPI::ClientId> _client,
        requestDataReply_t _reply) override
    {
        (void)_client;

        TelemetrySnapshotPlain plain{};

        if (!TelemetrySampler::sample(plain)) {
            v1::v1::logger::methods::TelemetryTypes::TelemetrySnapshot snap(
                "0",  // coreLoads
                0u,   // slot1 = temperatureC  → sending ram here
                0u    // slot2 = ramUsagePercentage → sending temp here
            );
            _reply(snap);
            return;
        }

        // Constructor: (coreLoads, temperatureC_slot, ramUsagePercentage_slot)
        // We intentionally swap so the receiver sees ram in temp field and temp in ram field
        v1::v1::logger::methods::TelemetryTypes::TelemetrySnapshot snap(
            plain.coreLoads,
            static_cast<uint16_t>(plain.ramUsagePercent),  // ram → temperatureC slot
            static_cast<uint32_t>(plain.temperatureC)      // temp → ramUsagePercentage slot
        );
        _reply(snap);
    }
};


int main() {
    auto runtime = CommonAPI::Runtime::get();
    if (!runtime) {
        std::cerr << "[Server] Failed to get CommonAPI runtime!\n";
        return 1;
    }

    auto stub = std::make_shared<TelemetryLoggingStub>();

    bool ok = runtime->registerService(
        "local",
        "logger.methods.justSendHi",
        stub
    );

    if (!ok) {
        std::cerr << "[Server] Failed to register service!" << std::endl;
        return 1;
    }

    std::cout << "[Server] Service registered, running..." << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}