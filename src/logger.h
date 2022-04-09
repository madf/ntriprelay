#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "log_writer.h"
#include "log_levels.h"

#include <sstream>
#include <ctime>

namespace MADF {

template <class Writer>
class Logger {
    public:
        Logger(LogLevel level = logInfo);
        ~Logger()
        {
            if (m_messageLevel >= m_logLevel)
                Writer::write(m_stream.str(), m_messageLevel);
        }

        std::ostream& stream() { return m_stream; }

        static void setLogLevel(LogLevel l) { m_logLevel = l; }
        static LogLevel getLogLevel() { return m_logLevel; }

    private:
        std::stringstream m_stream;

        LogLevel m_messageLevel;
        static LogLevel m_logLevel;

        void logTime();
};

template <>
inline
Logger<SysLogWriter>::Logger(LogLevel level)
    : m_messageLevel(level)
{
}

template <class Writer>
inline
Logger<Writer>::Logger(LogLevel level)
    : m_messageLevel(level)
{
    logTime();
}

template <class Writer>
inline
void Logger<Writer>::logTime()
{
    const time_t now(time(NULL));
    struct tm brokenTime;
    localtime_r(&now, &brokenTime);
    char buf[32];
    strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", &brokenTime);
    m_stream << buf << "\t";
}

}

#define LOG(writer, level) \
if (Logger<writer>::getLogLevel() > level) ; \
else Logger<writer>(level).stream()

#endif
