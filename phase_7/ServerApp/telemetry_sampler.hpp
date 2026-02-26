#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct TelemetrySnapshotPlain {
    std::string  coreLoads;  
    uint16_t     temperatureC;      
    uint32_t     ramUsagePercent;  
        
};

struct CpuTimes {
    uint64_t user    = 0;
    uint64_t nice    = 0;
    uint64_t system  = 0;
    uint64_t idle    = 0;
    uint64_t iowait  = 0;
    uint64_t irq     = 0;
    uint64_t softirq = 0;
    uint64_t steal   = 0;
};

class TelemetrySampler {
public:
    static bool sample(TelemetrySnapshotPlain &outSnapshot);

private:
    static bool readCpuTimes(std::vector<CpuTimes> &times);
    static std::vector<std::uint8_t> computePerCoreLoads();
    static std::string buildCoreLoadsString(const std::vector<std::uint8_t> &loads);
    static std::uint32_t readRamUsagePercent();
    static std::uint16_t readCpuTemperatureC();
};