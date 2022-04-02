#ifndef __CASTER_SETTINGS_H__
#define __CASTER_SETTINGS_H__

#include <boost/program_options.hpp>

#include <string>
#include <cstdint>

namespace po = boost::program_options;

namespace Caster {

class SettingsParser;
class Settings {
    public:
        Settings() noexcept;

        bool isHelp() const noexcept { return m_isHelp; }
        bool isVersion() const noexcept { return m_isVersion; }
        bool isDebug() const noexcept { return m_isDebug; }

        const std::string& sourceServer() const noexcept { return m_sourceServer; }
        const std::string& sourceMountpoint() const noexcept { return m_sourceMountpoint; }
        const std::string& sourceLogin() const noexcept { return m_sourceLogin; }
        const std::string& sourcePassword() const noexcept { return m_sourcePassword; }

        const std::string& destinationServer() const noexcept { return m_destinationServer; }
        const std::string& destinationMountpoint() const noexcept { return m_destinationMountpoint; }
        const std::string& destinationLogin() const noexcept { return m_destinationLogin; }
        const std::string& destinationPassword() const noexcept { return m_destinationPassword; }

        const std::string& gga() const noexcept { return m_gga; }

        int verbosity() const noexcept { return m_verbosity; }
        uint16_t destinationPort() const noexcept { return m_destinationPort; }
        uint16_t sourcePort() const noexcept { return m_sourcePort; }
        unsigned connectionTimeout() const noexcept { return m_connectionTimeout; }

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

class SettingsParser
{
    public:
        SettingsParser() noexcept;

        void init(int argc, char* argv[]);
        void printHelp() const noexcept;

        const Settings& settings() const noexcept { return m_settings; }
    private:
        po::options_description m_desc;
        Settings m_settings;
};

}

#endif
