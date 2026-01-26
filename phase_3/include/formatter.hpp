#pragma once
#include "logmessage.hpp"
#include "logtype.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <magic_enum/magic_enum.hpp>

template <typename Policy>
class LogFormatter
{
public:
static std::optional<logmessage> formatDataToLogMsg(const std::string &raw)
{
    auto valOpt = parseFloat(raw);
    if (!valOpt)
        return std::nullopt;

    float val = *valOpt;

    if (val < 0)
        return std::nullopt;

    if (val > Policy::maxValue)
        return std::nullopt;

    SeverityLvl_enum sevEnum = Policy::inferSeverity(val);

    std::string severityStr = std::string(magic_enum::enum_name(sevEnum));
    std::string contextStr  = std::string(magic_enum::enum_name(Policy::context));

    std::string description = msgDescription(val);
    std::string timestamp = currentTimeStamp();

    logmessage msg(
        "TelemetryApp",
        timestamp,
        contextStr,
        severityStr,
        description
    );

    return msg;
}


private:
    static std::string msgDescription(float val)
    {
        std::ostringstream ss;
        ss << magic_enum::enum_name(Policy::context) << " usage: " << val << Policy::unit;
        return ss.str();
    }

    static std::string currentTimeStamp()
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static std::optional<float> parseFloat(const std::string &raw)
    {
        try
        {
            return std::stof(raw);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }
};
 