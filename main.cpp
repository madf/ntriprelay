#include <csignal>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <exception>

#include <boost/system/error_code.hpp>

#include "logger.h"
#include "settings.h"
#include "version.h"
#include "error.h"
#include "client.h"

#define ERRLOG(level) LOG(CerrWriter, level)

using namespace MADF;
using namespace Caster;

void configureLogger(const SettingsParser & parser);
void printData(const boost::array<char, 1024> & data,
               size_t amount);
void printError(const boost::system::error_code & code);

int main(int argc, char * argv[])
{
    SettingsParser sParser;

    try {
        sParser.init(argc, argv);
    }
    catch (const CasterError & e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception & e) {
        std::cerr << "Fatal error!" << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }

    if (sParser.settings().isHelp()) {
        sParser.printHelp();
        return 0;
    }

    if (sParser.settings().isVersion()) {
        std::cout << "Boost NTRIP Client " << version << " (revision: " << revision << ")" << std::endl;
        return 0;
    }

    if (sParser.settings().server().empty()) {
        std::cerr << "You must specify at least server location" << std::endl;
        return -1;
    }

    configureLogger(sParser);

    if (sParser.settings().isDebug()) {
        std::cout << "Settings dump:\n"
                  << "\t- help: " << (sParser.settings().isHelp() ? "yes" : "no") << "\n"
                  << "\t- version: " << (sParser.settings().isVersion() ? "yes" : "no") << "\n"
                  << "\t- debug: " << (sParser.settings().isDebug() ? "yes" : "no") << "\n"
                  << "\t- server: " << sParser.settings().server() << "\n"
                  << "\t- mountpoint: " << sParser.settings().mountpoint() << "\n"
                  << "\t- login: " << sParser.settings().login() << "\n"
                  << "\t- password: " << sParser.settings().password() << "\n"
                  << "\t- GGA: " << sParser.settings().gga() << "\n"
                  << "\t- verbosity level: " << sParser.settings().verbosity() << "\n"
                  << "\t- port: " << sParser.settings().port() << "\n"
                  << "\t- connection timeout: " << sParser.settings().connectionTimeout() << std::endl;
    }

    try {
        assert(!sParser.settings().server().empty() && "Server must be specified!");
        boost::asio::io_service ioService;
        ClientPtr client;

        if (sParser.settings().mountpoint().empty()) {
            client.reset(new Client(ioService, sParser.settings().server(),
                                    sParser.settings().port()));
        } else {
            client.reset(new Client(ioService, sParser.settings().server(),
                                    sParser.settings().port(),
                                    sParser.settings().mountpoint()));
        }

        client->setDataCallback(printData);
        client->setErrorCallback(printError);
        if (!sParser.settings().login().empty() ||
            !sParser.settings().password().empty()) {
            client->setCredentials(sParser.settings().login(),
                                   sParser.settings().password());
        }
        if (!sParser.settings().gga().empty())
            client->setGGA(sParser.settings().gga());

        client->start(sParser.settings().connectionTimeout());

        ioService.run();
    }
    catch (CasterError & e) {
        ERRLOG(logError) << "Client error: " << e.what();
    }
    catch (std::exception & e) {
        ERRLOG(logFatal) << "System error: " << e.what();
        return -1;
    }

    return 0;
}

void configureLogger(const SettingsParser & parser)
{
    if (parser.settings().isDebug()) {
        switch (parser.settings().verbosity()) {
            case 0:
                Logger<CerrWriter>::setLogLevel(logInfo);
                break;
            case 1:
                Logger<CerrWriter>::setLogLevel(logDebug);
                break;
            case 2:
                Logger<CerrWriter>::setLogLevel(logAll);
                break;
        };
    } else {
        switch (parser.settings().verbosity()) {
            case 0:
                Logger<CerrWriter>::setLogLevel(logFatal);
                break;
            case 1:
                Logger<CerrWriter>::setLogLevel(logError);
                break;
            case 2:
                Logger<CerrWriter>::setLogLevel(logInfo);
                break;
        };
    }
}

void printData(const boost::array<char, 1024> & data,
               size_t amount)
{
    std::cout << std::string(data.begin(), data.begin() + amount);
}

void printError(const boost::system::error_code & code)
{
    ERRLOG(logError) << "Client error: " << code.message();
}
