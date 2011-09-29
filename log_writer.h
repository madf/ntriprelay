#ifndef __LOG_WRITER_H__
#define __LOG_WRITER_H__

#include <syslog.h>

#include <string>
#include <fstream>
#include <iostream>

#include "log_levels.h"

namespace MADF {

class NullWriter {
    public:
        static void write(const std::string &, LogLevel) {}
};

class CerrWriter {
    public:
        static void write(const std::string & message, LogLevel)
        { std::cerr << message << std::endl; }
};

class CoutWriter {
    public:
        static void write(const std::string & message, LogLevel)
        { std::cout << message << std::endl; }
};

class FileWriter {
    public:
        static bool open(const std::string & fileName,
                         std::ios_base::openmode mode = std::ofstream::app)
        {
            m_stream.open(fileName.c_str(), mode);
            return m_stream.is_open();
        }
        static void write(const std::string & message, LogLevel)
        { 
            if (m_stream)
                m_stream << message << std::endl;
        }

    private:
        static std::ofstream m_stream;
};

int SysLogLevel(LogLevel level);

class SysLogWriter {
    public:
        static void setFacility(int f) { m_facility = f; }
        static void write(const std::string & message, LogLevel level)
        {
            syslog(m_facility | SysLogLevel(level), "%s", message.c_str());
        }

    private:
        static int m_facility;
};

}

#endif
