#include "logger.h"

#include "log_levels.h"

using namespace MADF;

template <>
LogLevel Logger<NullWriter>::m_logLevel = logAll;

template <>
LogLevel Logger<CerrWriter>::m_logLevel = logAll;

template <>
LogLevel Logger<CoutWriter>::m_logLevel = logAll;

template <>
LogLevel Logger<FileWriter>::m_logLevel = logAll;

template <>
LogLevel Logger<SysLogWriter>::m_logLevel = logAll;
