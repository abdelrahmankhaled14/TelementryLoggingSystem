#include "logmessage.hpp"
#include "filesink.hpp"
#include "consolesink.hpp"
#include "logmanager.hpp"

int main()
{
    consolesink console;
    filesink fileSink;

    LogManager logger(&console);
    LogManager fileLogger(&fileSink);

    logmessage msg1("2026-01-06 17:00", "INFO", "Main", "Init", "Logging started");
    logmessage msg2("2026-01-06 17:01", "ERROR", "Main", "Config", "Failed to load config");
    logmessage msg3("2026-01-06 17:02", "DEBUG", "Main", "Process", "Processing data");

    logger.log(msg1);
    logger.log(msg2);
    fileLogger.log(msg3);
    logger.flush();
    fileLogger.flush();
    return 0;
}
