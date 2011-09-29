#ifndef __CASTER_SETTINGS_INL_H__
#define __CASTER_SETTINGS_INL_H__

#include <sstream>

namespace Caster {

inline
Settings::Settings() throw()
    : _isHelp(true),
      _isVersion(false),
      _isDebug(false),
      _sourceServer(),
      _sourceMountpoint(),
      _sourceLogin(),
      _sourcePassword(),
      _sourcePort(2101),
      _destinationServer(),
      _destinationMountpoint(),
      _destinationLogin(),
      _destinationPassword(),
      _destinationPort(2101),
      _gga(),
      _verbosity(1),
      _connectionTimeout(120)
{
}

inline
Settings::Settings(const Settings & rvalue) throw()
    : _isHelp(rvalue._isHelp),
      _isVersion(rvalue._isVersion),
      _isDebug(rvalue._isDebug),
      _sourceServer(rvalue._sourceServer),
      _sourceMountpoint(rvalue._sourceMountpoint),
      _sourceLogin(rvalue._sourceLogin),
      _sourcePassword(rvalue._sourcePassword),
      _sourcePort(rvalue._sourcePort),
      _destinationServer(rvalue._destinationServer),
      _destinationMountpoint(rvalue._destinationMountpoint),
      _destinationLogin(rvalue._destinationLogin),
      _destinationPassword(rvalue._destinationPassword),
      _destinationPort(rvalue._destinationPort),
      _gga(rvalue._gga),
      _verbosity(rvalue._verbosity),
      _connectionTimeout(rvalue._connectionTimeout)
{
}

inline
Settings::~Settings() throw()
{
}

inline
const Settings & Settings::operator=(const Settings & rvalue) throw()
{
    _isHelp = rvalue._isHelp;
    _isVersion = rvalue._isVersion;
    _isDebug = rvalue._isDebug;
    _sourceServer = rvalue._sourceServer;
    _sourceMountpoint = rvalue._sourceMountpoint;
    _sourceLogin = rvalue._sourceLogin;
    _sourcePassword = rvalue._sourcePassword;
    _sourcePort = rvalue._sourcePort;
    _destinationServer = rvalue._destinationServer;
    _destinationMountpoint = rvalue._destinationMountpoint;
    _destinationLogin = rvalue._destinationLogin;
    _destinationPassword = rvalue._destinationPassword;
    _destinationPort = rvalue._destinationPort;
    _gga = rvalue._gga;
    _verbosity = rvalue._verbosity;
    _connectionTimeout = rvalue._connectionTimeout;
    return *this;
}

inline
SettingsParser::SettingsParser() throw()
    : _desc("Allowed options")
{
    _desc.add_options()
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
SettingsParser::~SettingsParser() throw()
{
}

inline
void SettingsParser::printHelp() const throw()
{
    std::cout << _desc << std::endl;
}

}

#endif
