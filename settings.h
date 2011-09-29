#ifndef __CASTER_SETTINGS_H__
#define __CASTER_SETTINGS_H__

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>
#include <boost/cstdint.hpp>

namespace po = boost::program_options;

namespace Caster {

class SettingsParser;
class Settings {
    public:
        Settings() throw();
        Settings(const Settings & rvalue) throw();
        ~Settings() throw();

        const Settings & operator=(const Settings & rvalue) throw();
        
        bool isHelp() const throw() { return _isHelp; };
        bool isVersion() const throw() { return _isVersion; };
        bool isDebug() const throw() { return _isDebug; };

        const std::string & server() const throw() { return _server; };
        const std::string & mountpoint() const throw() { return _mountpoint; };
        const std::string & login() const throw() { return _login; };
        const std::string & password() const throw() { return _password; };
        const std::string & gga() const throw() { return _gga; };

        int verbosity() const throw() { return _verbosity; };
        uint16_t port() const throw() { return _port; };
        unsigned connectionTimeout() const throw() { return _connectionTimeout; };

    private:
        bool _isHelp;
        bool _isVersion;
        bool _isDebug;

        std::string _server;
        std::string _mountpoint;
        std::string _login;
        std::string _password;
        std::string _gga;

        int _verbosity;
        uint16_t _port;
        unsigned _connectionTimeout;

        friend class SettingsParser;
};

class SettingsParser : private boost::noncopyable {
    public:
        SettingsParser() throw();
        ~SettingsParser() throw();

        void init(int argc, char * argv[]);
        void printHelp() const throw();

        const Settings & settings() const throw() { return _settings; };
    private:
        po::options_description _desc;
        Settings _settings;
};

}

#include "settings.inl.h"

#endif
