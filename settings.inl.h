#ifndef __CASTER_SETTINGS_INL_H__
#define __CASTER_SETTINGS_INL_H__

#include <sstream>

namespace Caster {

inline
Settings::Settings() throw()
    : _isHelp(true),
      _isVersion(false),
      _isDebug(false),
      _server(),
      _mountpoint(),
      _login(),
      _password(),
      _gga(),
      _verbosity(1),
      _port(2101),
      _connectionTimeout(120)
{
}

inline
Settings::Settings(const Settings & rvalue) throw()
    : _isHelp(rvalue._isHelp),
      _isVersion(rvalue._isVersion),
      _isDebug(rvalue._isDebug),
      _server(rvalue._server),
      _mountpoint(rvalue._mountpoint),
      _login(rvalue._login),
      _password(rvalue._password),
      _gga(rvalue._gga),
      _verbosity(rvalue._verbosity),
      _port(rvalue._port),
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
    _server = rvalue._server;
    _mountpoint = rvalue._mountpoint;
    _login = rvalue._login;
    _password = rvalue._password;
    _gga = rvalue._gga;
    _verbosity = rvalue._verbosity;
    _port = rvalue._port;
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
        ("mountpoint,m", po::value<std::string>(), "mountpoint name")
        ("login,l", po::value<std::string>(), "login")
        ("password,w", po::value<std::string>(), "password")
        ("port,p", po::value<std::string>(), "server port")
        ("server,s", po::value<std::string>(), "server address")
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
