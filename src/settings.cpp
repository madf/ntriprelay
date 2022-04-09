#include "settings.h"
#include "error.h"

#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <iostream>

using Caster::Settings;
using Caster::SettingsParser;

Settings::Settings() noexcept
    : m_isHelp(true),
      m_isVersion(false),
      m_isDebug(false),
      m_sourcePort(2101),
      m_destinationPort(2101),
      m_verbosity(1),
      m_connectionTimeout(120)
{
}

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

void SettingsParser::init(int argc, char* argv[])
{
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, m_desc), vm);
    po::notify(vm);

    m_settings.m_isHelp = vm.count("help") > 0;
    m_settings.m_isVersion = vm.count("version") > 0;

    if (vm.count("debug") > 0)
        m_settings.m_isDebug = true;

    if (vm.count("src-server") > 0)
        m_settings.m_sourceServer = vm["src-server"].as<std::string>();

    if (vm.count("src-mountpoint") > 0)
        m_settings.m_sourceMountpoint = vm["src-mountpoint"].as<std::string>();

    if (vm.count("src-login") > 0)
        m_settings.m_sourceLogin = vm["src-login"].as<std::string>();

    if (vm.count("src-password") > 0)
        m_settings.m_sourcePassword = vm["src-password"].as<std::string>();

    if (vm.count("src-port") > 0)
    {
        try
        {
            m_settings.m_sourcePort = vm["src-port"].as<uint16_t>();
        }
        catch (boost::bad_lexical_cast &)
        {
            throw CasterError("Invalid source port value");
        }
    }

    if (vm.count("dst-server") > 0)
        m_settings.m_destinationServer = vm["dst-server"].as<std::string>();

    if (vm.count("dst-mountpoint") > 0)
        m_settings.m_destinationMountpoint = vm["dst-mountpoint"].as<std::string>();

    if (vm.count("dst-login") > 0)
        m_settings.m_destinationLogin = vm["dst-login"].as<std::string>();

    if (vm.count("dst-password") > 0)
        m_settings.m_destinationPassword = vm["dst-password"].as<std::string>();

    if (vm.count("dst-port") > 0)
    {
        try
        {
            m_settings.m_destinationPort = vm["dst-port"].as<uint16_t>();
        }
        catch (boost::bad_lexical_cast &)
        {
            throw CasterError("Invalid destination port value");
        }
    }

    if (vm.count("verbosity") > 0)
    {
        m_settings.m_verbosity = vm["verbosity"].as<int>();
        if (m_settings.m_verbosity < 1)
        {
            m_settings.m_verbosity = 0;
        }
        else if (m_settings.m_verbosity > 1)
        {
            m_settings.m_verbosity = 2;
        }
    }

    if (vm.count("timeout") > 0)
        m_settings.m_connectionTimeout = vm["timeout"].as<unsigned>();

    if (vm.count("gga") > 0)
        m_settings.m_gga = vm["gga"].as<std::string>();
}

void SettingsParser::printHelp() const noexcept
{
    std::cout << m_desc << std::endl;
}
