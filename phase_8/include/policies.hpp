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
    static constexpr float maxValue = 100.0f;
    static constexpr int WARNING = 70;
    static constexpr float CRITICAL = 85;
    static constexpr std::string_view unit = "%";
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
struct TempPolicy
{
    static constexpr float maxValue = 120.0f;          // max safe CPU temp
    static constexpr float WARNING   = 70.0f;          // Yellow zone
    static constexpr float CRITICAL  = 85.0f;          // Red zone
    static constexpr std::string_view unit = "°C";
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::TEMP;

    static constexpr SeverityLvl_enum inferSeverity(float val)
    {
        if (val > CRITICAL)
            return SeverityLvl_enum::CRITICAL;
        if (val > WARNING)
            return SeverityLvl_enum::WARNING;
        return SeverityLvl_enum::INFO;
    }
};