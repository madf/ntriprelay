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
        
        bool isHelp() const throw() { return _isHelp; }
        bool isVersion() const throw() { return _isVersion; }
        bool isDebug() const throw() { return _isDebug; }

        const std::string & sourceServer() const throw() { return _sourceServer; }
        const std::string & sourceMountpoint() const throw() { return _sourceMountpoint; }
        const std::string & sourceLogin() const throw() { return _sourceLogin; }
        const std::string & sourcePassword() const throw() { return _sourcePassword; }

        const std::string & destinationServer() const throw() { return _destinationServer; }
        const std::string & destinationMountpoint() const throw() { return _destinationMountpoint; }
        const std::string & destinationLogin() const throw() { return _destinationLogin; }
        const std::string & destinationPassword() const throw() { return _destinationPassword; }

        const std::string & gga() const throw() { return _gga; }

        int verbosity() const throw() { return _verbosity; }
        uint16_t destinationPort() const throw() { return _destinationPort; }
        uint16_t sourcePort() const throw() { return _sourcePort; }
        unsigned connectionTimeout() const throw() { return _connectionTimeout; }

    private:
        bool _isHelp;
        bool _isVersion;
        bool _isDebug;

        std::string _sourceServer;
        std::string _sourceMountpoint;
        std::string _sourceLogin;
        std::string _sourcePassword;
        uint16_t _sourcePort;
        std::string _destinationServer;
        std::string _destinationMountpoint;
        std::string _destinationLogin;
        std::string _destinationPassword;
        uint16_t _destinationPort;
        std::string _gga;

        int _verbosity;
        unsigned _connectionTimeout;

        friend class SettingsParser;
};

class SettingsParser : private boost::noncopyable {
    public:
        SettingsParser() throw();
        ~SettingsParser() throw();

        void init(int argc, char * argv[]);
        void printHelp() const throw();

        const Settings & settings() const throw() { return _settings; }
    private:
        po::options_description _desc;
        Settings _settings;
};

}

#include "settings.inl.h"

#endif
