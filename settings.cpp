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

    if (vm.count("server")) {
        _settings._server = vm["server"].as<std::string>();
    }

    if (vm.count("mountpoint")) {
        _settings._mountpoint = vm["mountpoint"].as<std::string>();
    }

    if (vm.count("login")) {
        _settings._login = vm["login"].as<std::string>();
    }

    if (vm.count("password")) {
        _settings._password = vm["password"].as<std::string>();
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

    if (vm.count("port")) {
        try {
            _settings._port = boost::lexical_cast<uint16_t>(vm["port"].as<std::string>());
        }
        catch (boost::bad_lexical_cast &) {
            throw CasterError("Invalid port value");
        }
    }
}
