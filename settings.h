#ifndef __CASTER_SETTINGS_H__
#define __CASTER_SETTINGS_H__

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>
#include <boost/cstdint.hpp>

#include <string>

namespace po = boost::program_options;

namespace Caster {

class SettingsParser;
class Settings {
    public:
        Settings() throw();
        Settings(const Settings & rvalue) throw();
        ~Settings() throw();

        const Settings& operator=(const Settings & rvalue) throw();

        bool isHelp() const throw() { return m_isHelp; }
        bool isVersion() const throw() { return m_isVersion; }
        bool isDebug() const throw() { return m_isDebug; }

        const std::string& sourceServer() const throw() { return m_sourceServer; }
        const std::string& sourceMountpoint() const throw() { return m_sourceMountpoint; }
        const std::string& sourceLogin() const throw() { return m_sourceLogin; }
        const std::string& sourcePassword() const throw() { return m_sourcePassword; }

        const std::string& destinationServer() const throw() { return m_destinationServer; }
        const std::string& destinationMountpoint() const throw() { return m_destinationMountpoint; }
        const std::string& destinationLogin() const throw() { return m_destinationLogin; }
        const std::string& destinationPassword() const throw() { return m_destinationPassword; }

        const std::string& gga() const throw() { return m_gga; }

        int verbosity() const throw() { return m_verbosity; }
        uint16_t destinationPort() const throw() { return m_destinationPort; }
        uint16_t sourcePort() const throw() { return m_sourcePort; }
        unsigned connectionTimeout() const throw() { return m_connectionTimeout; }

    private:
        bool m_isHelp;
        bool m_isVersion;
        bool m_isDebug;

        std::string m_sourceServer;
        std::string m_sourceMountpoint;
        std::string m_sourceLogin;
        std::string m_sourcePassword;
        uint16_t m_sourcePort;
        std::string m_destinationServer;
        std::string m_destinationMountpoint;
        std::string m_destinationLogin;
        std::string m_destinationPassword;
        uint16_t m_destinationPort;
        std::string m_gga;

        int m_verbosity;
        unsigned m_connectionTimeout;

        friend class SettingsParser;
};

class SettingsParser : private boost::noncopyable {
    public:
        SettingsParser() throw();
        ~SettingsParser() throw();

        void init(int argc, char* argv[]);
        void printHelp() const throw();

        const Settings& settings() const throw() { return m_settings; }
    private:
        po::options_description m_desc;
        Settings m_settings;
};

}

#include "settings.inl.h"

#endif
