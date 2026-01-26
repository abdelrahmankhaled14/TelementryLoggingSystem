#pragma once
#include "logtype.hpp"
#include <string_view>

struct CpuPolicy
{
    static constexpr float maxValue = 100.0f;
    static constexpr float WARNING = 75.0f;
    static constexpr float CRITICAL = 90.0f;
    static constexpr std::string_view unit = "%";
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;

    static constexpr SeverityLvl_enum inferSeverity(float val)
    {
        if (val > CRITICAL)
            return SeverityLvl_enum::CRITICAL;
        if (val > WARNING)
            return SeverityLvl_enum::WARNING;
        return SeverityLvl_enum::INFO;
    }
};
struct RamPolicy
{
    static constexpr float maxValue = 16.0f;
    static constexpr int WARNING = 8;
    static constexpr float CRITICAL = 12;
    static constexpr std::string_view unit = "GB";
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::RAM;
    static constexpr SeverityLvl_enum inferSeverity(float val)
    {
        if (val > CRITICAL)
            return SeverityLvl_enum::CRITICAL;
        if (val > WARNING)
            return SeverityLvl_enum::WARNING;
        return SeverityLvl_enum::INFO;
    }
};