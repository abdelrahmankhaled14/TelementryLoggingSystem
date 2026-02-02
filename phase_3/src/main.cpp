#include "FileTelemetrySourceImpl.hpp"
#include "consolesink.hpp"
#include "filesink.hpp"
#include "formatter.hpp"
#include "policies.hpp"
#include "logmessage.hpp"
#include "logmanager.hpp"
#include <iostream>
#include <memory>
#include <optional>
int main()
{
    std::unique_ptr<ITelemetrySource> source = 
        std::make_unique<FileTelemetrySourceImpl>("telemetry.txt");
    
    if (!source->OpenSource()) {
        std::cerr << "Failed to open telemetry source file.\n";
        return 1;
    }

    std::unique_ptr<Ilogsink> consoleSink = std::make_unique<consolesink>();
    std::unique_ptr<Ilogsink> fileSink = 
        std::make_unique<filesink>();

    logmanagerbuilder builder(10);

    builder.addSink(consoleSink.get()).addSink(fileSink.get());
    LogManager logManager = builder.build();

    std::string rawLine;
    
    while (source->ReadSource(rawLine))
    {
        auto msgOpt = LogFormatter<RamPolicy>::formatDataToLogMsg(rawLine);
        if (msgOpt) 
        {
            logManager.log(*msgOpt);
        }
    }

    logManager.flush();
    std::cout << "Total messages processed: " << logManager.getMessageCount() << "\n";

    return 0;
}