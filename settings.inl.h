#ifndef __CASTER_SETTINGS_INL_H__
#define __CASTER_SETTINGS_INL_H__

#include <sstream>
#include <iostream>

namespace Caster {

inline
Settings::Settings() noexcept
    : m_isHelp(true),
      m_isVersion(false),
      m_isDebug(false),
      m_sourceServer(),
      m_sourceMountpoint(),
      m_sourceLogin(),
      m_sourcePassword(),
      m_sourcePort(2101),
      m_destinationServer(),
      m_destinationMountpoint(),
      m_destinationLogin(),
      m_destinationPassword(),
      m_destinationPort(2101),
      m_gga(),
      m_verbosity(1),
      m_connectionTimeout(120)
{
}

inline
Settings::Settings(const Settings& rvalue) noexcept
    : m_isHelp(rvalue.m_isHelp),
      m_isVersion(rvalue.m_isVersion),
      m_isDebug(rvalue.m_isDebug),
      m_sourceServer(rvalue.m_sourceServer),
      m_sourceMountpoint(rvalue.m_sourceMountpoint),
      m_sourceLogin(rvalue.m_sourceLogin),
      m_sourcePassword(rvalue.m_sourcePassword),
      m_sourcePort(rvalue.m_sourcePort),
      m_destinationServer(rvalue.m_destinationServer),
      m_destinationMountpoint(rvalue.m_destinationMountpoint),
      m_destinationLogin(rvalue.m_destinationLogin),
      m_destinationPassword(rvalue.m_destinationPassword),
      m_destinationPort(rvalue.m_destinationPort),
      m_gga(rvalue.m_gga),
      m_verbosity(rvalue.m_verbosity),
      m_connectionTimeout(rvalue.m_connectionTimeout)
{
}

inline
const Settings& Settings::operator=(const Settings& rvalue) noexcept
{
    m_isHelp = rvalue.m_isHelp;
    m_isVersion = rvalue.m_isVersion;
    m_isDebug = rvalue.m_isDebug;
    m_sourceServer = rvalue.m_sourceServer;
    m_sourceMountpoint = rvalue.m_sourceMountpoint;
    m_sourceLogin = rvalue.m_sourceLogin;
    m_sourcePassword = rvalue.m_sourcePassword;
    m_sourcePort = rvalue.m_sourcePort;
    m_destinationServer = rvalue.m_destinationServer;
    m_destinationMountpoint = rvalue.m_destinationMountpoint;
    m_destinationLogin = rvalue.m_destinationLogin;
    m_destinationPassword = rvalue.m_destinationPassword;
    m_destinationPort = rvalue.m_destinationPort;
    m_gga = rvalue.m_gga;
    m_verbosity = rvalue.m_verbosity;
    m_connectionTimeout = rvalue.m_connectionTimeout;
    return *this;
}

inline
SettingsParser::SettingsParser() noexcept
    : m_desc("Allowed options")
{
    m_desc.add_options()
        ("help,h", "produce this help message")
        ("debug,d", "NTRIP clinet debugging")
        ("gga,g", po::value<std::string>(), "GPGGA string")
        ("src-mountpoint,M", po::value<std::string>(), "source mountpoint name")
        ("src-login,L", po::value<std::string>(), "source login")
        ("src-password,W", po::value<std::string>(), "source password")
        ("src-port,P", po::value<uint16_t>(), "source server port")
        ("src-server,S", po::value<std::string>(), "source server address")
        ("dst-mountpoint,m", po::value<std::string>(), "destination mountpoint name")
        ("dst-login,l", po::value<std::string>(), "destination login")
        ("dst-password,w", po::value<std::string>(), "destination password")
        ("dst-port,p", po::value<uint16_t>(), "destination server port")
        ("dst-server,s", po::value<std::string>(), "destination server address")
        ("timeout,t", po::value<unsigned>(), "connection timeout")
        ("verbosity,V", po::value<int>(), "log file verbosity (0 - quiet, 1 - normal, 2 - extra)")
        ("version,v", "show NTRIP client version and exit")
    ;
}

inline
void SettingsParser::printHelp() const noexcept
{
    std::cout << m_desc << std::endl;
}

}

#endif
