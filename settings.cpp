#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "error.h"
#include "settings.h"

using Caster::SettingsParser;

void SettingsParser::init(int argc, char * argv[])
{
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, _desc), vm);
    po::notify(vm);    

    _settings._isHelp = vm.count("help");
    _settings._isVersion = vm.count("version");

    if (vm.count("debug")) {
        _settings._isDebug = true;
    }

    if (vm.count("src-server")) {
        _settings._sourceServer = vm["src-server"].as<std::string>();
    }

    if (vm.count("src-mountpoint")) {
        _settings._sourceMountpoint = vm["src-mountpoint"].as<std::string>();
    }

    if (vm.count("src-login")) {
        _settings._sourceLogin = vm["src-login"].as<std::string>();
    }

    if (vm.count("src-password")) {
        _settings._sourcePassword = vm["src-password"].as<std::string>();
    }

    if (vm.count("src-port")) {
        try {
            _settings._sourcePort = vm["src-port"].as<uint16_t>();
        }
        catch (boost::bad_lexical_cast &) {
            throw CasterError("Invalid source port value");
        }
    }

    if (vm.count("dst-server")) {
        _settings._destinationServer = vm["dst-server"].as<std::string>();
    }

    if (vm.count("dst-mountpoint")) {
        _settings._destinationMountpoint = vm["dst-mountpoint"].as<std::string>();
    }

    if (vm.count("dst-login")) {
        _settings._destinationLogin = vm["dst-login"].as<std::string>();
    }

    if (vm.count("dst-password")) {
        _settings._destinationPassword = vm["dst-password"].as<std::string>();
    }

    if (vm.count("dst-port")) {
        try {
            _settings._destinationPort = vm["dst-port"].as<uint16_t>();
        }
        catch (boost::bad_lexical_cast &) {
            throw CasterError("Invalid destination port value");
        }
    }

    if (vm.count("verbosity")) {
        _settings._verbosity = vm["verbosity"].as<int>();
        if (_settings._verbosity < 1) {
            _settings._verbosity = 0;
        } else if (_settings._verbosity > 1) {
            _settings._verbosity = 2;
        }
    }

    if (vm.count("timeout")) {
        _settings._connectionTimeout = vm["timeout"].as<unsigned>();
    }

    if (vm.count("gga")) {
        _settings._gga = vm["gga"].as<std::string>();
    }
}
