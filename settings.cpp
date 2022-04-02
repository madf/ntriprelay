#include "settings.h"
#include "error.h"

#include <boost/lexical_cast.hpp>

#include <stdexcept>

using Caster::SettingsParser;

void SettingsParser::init(int argc, char* argv[])
{
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, m_desc), vm);
    po::notify(vm);

    m_settings.m_isHelp = vm.count("help");
    m_settings.m_isVersion = vm.count("version");

    if (vm.count("debug")) {
        m_settings.m_isDebug = true;
    }

    if (vm.count("src-server")) {
        m_settings.m_sourceServer = vm["src-server"].as<std::string>();
    }

    if (vm.count("src-mountpoint")) {
        m_settings.m_sourceMountpoint = vm["src-mountpoint"].as<std::string>();
    }

    if (vm.count("src-login")) {
        m_settings.m_sourceLogin = vm["src-login"].as<std::string>();
    }

    if (vm.count("src-password")) {
        m_settings.m_sourcePassword = vm["src-password"].as<std::string>();
    }

    if (vm.count("src-port")) {
        try {
            m_settings.m_sourcePort = vm["src-port"].as<uint16_t>();
        }
        catch (boost::bad_lexical_cast &) {
            throw CasterError("Invalid source port value");
        }
    }

    if (vm.count("dst-server")) {
        m_settings.m_destinationServer = vm["dst-server"].as<std::string>();
    }

    if (vm.count("dst-mountpoint")) {
        m_settings.m_destinationMountpoint = vm["dst-mountpoint"].as<std::string>();
    }

    if (vm.count("dst-login")) {
        m_settings.m_destinationLogin = vm["dst-login"].as<std::string>();
    }

    if (vm.count("dst-password")) {
        m_settings.m_destinationPassword = vm["dst-password"].as<std::string>();
    }

    if (vm.count("dst-port")) {
        try {
            m_settings.m_destinationPort = vm["dst-port"].as<uint16_t>();
        }
        catch (boost::bad_lexical_cast &) {
            throw CasterError("Invalid destination port value");
        }
    }

    if (vm.count("verbosity")) {
        m_settings.m_verbosity = vm["verbosity"].as<int>();
        if (m_settings.m_verbosity < 1) {
            m_settings.m_verbosity = 0;
        } else if (m_settings.m_verbosity > 1) {
            m_settings.m_verbosity = 2;
        }
    }

    if (vm.count("timeout")) {
        m_settings.m_connectionTimeout = vm["timeout"].as<unsigned>();
    }

    if (vm.count("gga")) {
        m_settings.m_gga = vm["gga"].as<std::string>();
    }
}
