// telemetry_sampler.cpp
#include "telemetry_sampler.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

static const char *PROC_STAT_PATH    = "/proc/stat";
static const char *PROC_MEMINFO      = "/proc/meminfo";
// adjust if your CPU temp is in a different zone
static const char *THERMAL_TEMP_PATH = "/sys/class/thermal/thermal_zone5/temp";

bool TelemetrySampler::sample(TelemetrySnapshotPlain &outSnapshot) {
    // --- CPU loads ---
    std::vector<std::uint8_t> coreLoads = computePerCoreLoads();
    outSnapshot.coreLoads = buildCoreLoadsString(coreLoads);

    // --- RAM ---
    outSnapshot.ramUsagePercent = readRamUsagePercent();

    // --- Temperature ---
    outSnapshot.temperatureC = readCpuTemperatureC();

    // best-effort: always return true; tighten if needed
    return true;
}

// ---------------- CPU helpers ----------------

bool TelemetrySampler::readCpuTimes(std::vector<CpuTimes> &times) {
    times.clear();

    std::ifstream file(PROC_STAT_PATH);
    if (!file.is_open()) {
        std::cerr << "[Telemetry] Failed to open " << PROC_STAT_PATH << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("cpu", 0) != 0) {
            continue;
        }
        // Skip the aggregate "cpu " line; we only want cpu0, cpu1, ...
        if (line.size() > 3 && std::isdigit(static_cast<unsigned char>(line[3]))) {
            std::istringstream iss(line);
            std::string label;
            CpuTimes ct;
            iss >> label
                >> ct.user
                >> ct.nice
                >> ct.system
                >> ct.idle
                >> ct.iowait
                >> ct.irq
                >> ct.softirq
                >> ct.steal;
            times.push_back(ct);
        }
    }

    return !times.empty();
}

std::vector<std::uint8_t> TelemetrySampler::computePerCoreLoads() {
    std::vector<CpuTimes> t1, t2;
    std::vector<std::uint8_t> result;

    if (!readCpuTimes(t1)) {
        return result;
    }

    // small delay for measuring the delta
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (!readCpuTimes(t2)) {
        return result;
    }

    const std::size_t cores = std::min(t1.size(), t2.size());
    result.resize(cores, 0);

    for (std::size_t i = 0; i < cores; ++i) {
        const CpuTimes &a = t1[i];
        const CpuTimes &b = t2[i];

        std::uint64_t idle1    = a.idle + a.iowait;
        std::uint64_t idle2    = b.idle + b.iowait;
        std::uint64_t nonIdle1 = a.user + a.nice + a.system + a.irq + a.softirq + a.steal;
        std::uint64_t nonIdle2 = b.user + b.nice + b.system + b.irq + b.softirq + b.steal;
        std::uint64_t total1   = idle1 + nonIdle1;
        std::uint64_t total2   = idle2 + nonIdle2;
        std::uint64_t totald   = (total2 > total1) ? (total2 - total1) : 0;
        std::uint64_t idled    = (idle2  > idle1)  ? (idle2  - idle1)  : 0;

        std::uint8_t usage = 0;
        if (totald > 0) {
            double pct = static_cast<double>(totald - idled) * 100.0
                       / static_cast<double>(totald);
            if (pct < 0.0)   pct = 0.0;
            if (pct > 100.0) pct = 100.0;
            usage = static_cast<std::uint8_t>(pct + 0.5); // round
        }

        result[i] = usage;
    }

    return result;
}

std::string TelemetrySampler::buildCoreLoadsString(const std::vector<std::uint8_t> &loads) {
    if (loads.empty()) {
        return "0";
    }

    std::uint64_t sum = 0;
    for (auto v : loads) {
        sum += v;
    }
    std::uint8_t avg = static_cast<std::uint8_t>(
        std::min<std::uint64_t>(100, sum / loads.size())
    );

    // Return (avg + 1) * 5, capped at 100
    int scaled = std::min(100, (static_cast<int>(avg) + 1) * 5);
    return std::to_string(scaled);
}

// ---------------- RAM helper ----------------

std::uint32_t TelemetrySampler::readRamUsagePercent() {
    std::ifstream file(PROC_MEMINFO);
    if (!file.is_open()) {
        std::cerr << "[Telemetry] Failed to open " << PROC_MEMINFO << "\n";
        return 0;
    }

    std::string   line;
    std::uint64_t memTotal     = 0;
    std::uint64_t memAvailable = 0;

    while (std::getline(file, line)) {
        if (line.rfind("MemTotal:", 0) == 0) {
            std::istringstream iss(line.substr(9));
            iss >> memTotal;
        } else if (line.rfind("MemAvailable:", 0) == 0) {
            std::istringstream iss(line.substr(13));
            iss >> memAvailable;
        }
        if (memTotal && memAvailable) {
            break;
        }
    }

    if (memTotal == 0) {
        return 0;
    }

    std::uint64_t used    = memTotal - memAvailable;
    std::uint64_t percent = (used * 100) / memTotal;
    if (percent > 100) percent = 100;

    return static_cast<std::uint32_t>(percent);
}

// ---------------- Temp helper ----------------

std::uint16_t TelemetrySampler::readCpuTemperatureC() {
    std::ifstream file(THERMAL_TEMP_PATH);
    if (!file.is_open()) {
        std::cerr << "[Telemetry] Failed to open " << THERMAL_TEMP_PATH << "\n";
        return 0;
    }

    long raw = 0;
    file >> raw;

    // Usually in millidegrees C (e.g. 55000 -> 55°C)
    if (raw < 0) raw = 0;
    long c = raw / 1000;
    if (c < 0)     c = 0;
    if (c > 65535) c = 65535;

    return static_cast<std::uint16_t>(c);
}