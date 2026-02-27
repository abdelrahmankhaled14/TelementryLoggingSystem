// config.cpp
#include "config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

using nlohmann::json;

AppConfig loadConfig(const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("Failed to open config file: " + path);
    }

    json j;
    ifs >> j;

    AppConfig cfg;

    auto js = j["source"];
    cfg.sourceType = js.at("type").get<std::string>();

    if (cfg.sourceType == "someip") {
        if (js.contains("mapping")) {
            cfg.mapping = js["mapping"].get<std::vector<std::string>>();
        }
    } else {
        cfg.policy = js.value("policy", "cpu");
    }

    auto sk = j["sinks"];
    cfg.sinkConsole = sk.value("console", true);
    cfg.sinkCpuFile = sk.value("cpu_file", true);
    cfg.sinkRamFile = sk.value("ram_file", true);
    cfg.sinkTempFile = sk.value("temp_file", true);

    auto rt = j["rates"];
    cfg.parseMs = rt.value("parse_ms", 0);
    cfg.logMs   = rt.value("log_ms", 0);

    return cfg;
}