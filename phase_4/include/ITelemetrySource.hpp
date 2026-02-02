#pragma once 
#include <iostream>
class ITelemetrySource
{
private:

public:
    ITelemetrySource() = default;
    virtual bool OpenSource() = 0;
    virtual bool ReadSource(std::string & out) = 0;
    virtual ~ITelemetrySource() = default;
};


