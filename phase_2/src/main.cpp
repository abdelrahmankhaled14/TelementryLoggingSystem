// #include "logmessage.hpp"
// #include "filesink.hpp"
// #include "consolesink.hpp"
// #include "logmanager.hpp"
// #include "FileTelemetrySourceImpl.hpp"

// int main()
// {
//     // consolesink console;
//     // filesink fileSink;

//     // LogManager logger(&console);
//     // LogManager fileLogger(&fileSink);

//     // logmessage msg1("2026-01-06 17:00", "INFO", "Main", "Init", "Logging started");
//     // logmessage msg2("2026-01-06 17:01", "ERROR", "Main", "Config", "Failed to load config");
//     // logmessage msg3("2026-01-06 17:02", "DEBUG", "Main", "Process", "Processing data");

//     // logger.log(msg1);
//     // logger.log(msg2);
//     // fileLogger.log(msg3);
//     // logger.flush();
//     // fileLogger.flush();
//     // return 0;
// }


#include "FileTelemetrySourceImpl.hpp"
#include <iostream>

int main()
{
    std::string out;
    
    FileTelemetrySourceImpl cpuSource("/proc/meminfo");
    
   
    ITelemetrySource* source = &cpuSource;  
    
    if (!source->OpenSource()) {
        std::cerr << "Failed to open /proc/meminfo" << std::endl;
        return 1;
    }
    
    if (!source->ReadSource(out)) {
        std::cerr << "Failed to read /proc/meminfo" << std::endl;
        return 1;
    }
    
    std::cout << "CPU Stats:\n" << out << std::endl;
    
    
    return 0;
}
