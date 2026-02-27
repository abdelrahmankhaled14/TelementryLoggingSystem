// config.hpp
#pragma once
#include <string>
#include <vector>

struct AppConfig {
    std::string sourceType;          // "someip" | "file" | "socket"
    std::vector<std::string> mapping; // for someip: e.g. ["cpu","temp","ram"]
    std::string policy;             // for file/socket: "cpu"|"ram"|"temp"

    bool sinkConsole{true};
    bool sinkCpuFile{true};
    bool sinkRamFile{true};
    bool sinkTempFile{true};

    int parseMs{0};
    int logMs{0};
};

AppConfig loadConfig(const std::string& path);