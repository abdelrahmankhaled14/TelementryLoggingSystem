#pragma once

#include "ITelemetrySource.hpp"
#include "safefile.hpp"
#include <string>
#include <memory>

class FileTelemetrySourceImpl : public ITelemetrySource {
private:
    std::string path_;
    std::unique_ptr<safefile> file_;
    std::string leftover_; // store incomplete line
    
public:
    explicit FileTelemetrySourceImpl(const std::string& path);
    
    bool OpenSource() override;
    bool ReadSource(std::string& out) override;
};
