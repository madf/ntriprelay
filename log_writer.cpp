#include <fstream>

#include "log_writer.h"

using namespace MADF;

std::ofstream FileWriter::m_stream;
int SysLogWriter::m_facility = LOG_USER;

int MADF::SysLogLevel(LogLevel level)
{
    switch (level) {
        case logAll:
            return LOG_DEBUG;
        case logDebug:
            return LOG_DEBUG;
        case logInfo:
            return LOG_INFO;
        case logWarning:
            return LOG_WARNING;
        case logError:
            return LOG_ERR;
        case logFatal:
            return LOG_CRIT;
        case logNone:
            return 0;
    }

    return 0;
}
