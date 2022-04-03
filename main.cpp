#include "relay.h"
#include "logger.h"
#include "settings.h"
#include "version.h"
#include "error.h"

#include <boost/system/error_code.hpp>

#include <iostream>
#include <functional> // std::bind
#include <exception>
#include <csignal>
#include <cerrno>
#include <cstring>

#define ERRLOG(level) LOG(CerrWriter, level)

using namespace MADF;
using namespace Caster;

void configureLogger(const SettingsParser& parser);
void printError(const boost::system::error_code& code);
void printHeaders(const RelayPtr& relayPtr);

int main(int argc, char* argv[])
{
    SettingsParser sParser;

    try {
        sParser.init(argc, argv);
    }
    catch (const CasterError& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error!" << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }

    if (sParser.settings().isHelp()) {
        sParser.printHelp();
        return 0;
    }

    if (sParser.settings().isVersion()) {
        std::cout << "Boost NTRIP Relay " << version << " (revision: " << revision << ")" << std::endl;
        return 0;
    }

    if (sParser.settings().sourceServer().empty()) {
        std::cerr << "You must specify source server location" << std::endl;
        return -1;
    }

    if (sParser.settings().destinationServer().empty()) {
        std::cerr << "You must specify source server location" << std::endl;
        return -1;
    }

    configureLogger(sParser);

    if (sParser.settings().isDebug()) {
        std::cout << "Settings dump:\n"
                  << "\t- connection timeout: " << sParser.settings().connectionTimeout() << "\n"
                  << "\t- debug: " << (sParser.settings().isDebug() ? "yes" : "no") << "\n"
                  << "\t- destination login: " << sParser.settings().destinationLogin() << "\n"
                  << "\t- destination mountpoint: " << sParser.settings().destinationMountpoint() << "\n"
                  << "\t- destination password: " << sParser.settings().destinationPassword() << "\n"
                  << "\t- destination port: " << sParser.settings().destinationPort() << "\n"
                  << "\t- destination server: " << sParser.settings().destinationServer() << "\n"
                  << "\t- GGA: " << sParser.settings().gga() << "\n"
                  << "\t- help: " << (sParser.settings().isHelp() ? "yes" : "no") << "\n"
                  << "\t- source login: " << sParser.settings().sourceLogin() << "\n"
                  << "\t- source mountpoint: " << sParser.settings().sourceMountpoint() << "\n"
                  << "\t- source password: " << sParser.settings().sourcePassword() << "\n"
                  << "\t- source port: " << sParser.settings().sourcePort() << "\n"
                  << "\t- source server: " << sParser.settings().sourceServer() << "\n"
                  << "\t- verbosity level: " << sParser.settings().verbosity() << "\n"
                  << "\t- version: " << (sParser.settings().isVersion() ? "yes" : "no") << std::endl;
    }

    try {
        boost::asio::io_service ioService;
        RelayPtr relay;

        relay.reset(new Relay(ioService,
                              sParser.settings().sourceServer(),
                              sParser.settings().sourcePort(),
                              sParser.settings().sourceMountpoint(),
                              sParser.settings().destinationServer(),
                              sParser.settings().destinationPort(),
                              sParser.settings().destinationMountpoint()));

        relay->setErrorCallback(printError);
        relay->setHeadersCallback(std::bind(printHeaders, relay));
        if (!sParser.settings().sourceLogin().empty() ||
            !sParser.settings().sourcePassword().empty()) {
            relay->setSrcCredentials(sParser.settings().sourceLogin(),
                                        sParser.settings().sourcePassword());
        }
        if (!sParser.settings().destinationLogin().empty() ||
            !sParser.settings().destinationPassword().empty()) {
            relay->setDstCredentials(sParser.settings().destinationLogin(),
                                             sParser.settings().destinationPassword());
        }
        if (!sParser.settings().gga().empty())
            relay->setGGA(sParser.settings().gga());

        ERRLOG(logDebug) << "Before starting...";

        relay->start(sParser.settings().connectionTimeout());

        ERRLOG(logDebug) << "Starting...";

        ioService.run();

        ERRLOG(logDebug) << "Stopping...";
    }
    catch (const CasterError& e) {
        ERRLOG(logError) << "Relay error: " << e.what();
    }
    catch (const std::exception& e) {
        ERRLOG(logFatal) << "System error: " << e.what();
        return -1;
    }

    return 0;
}

void configureLogger(const SettingsParser& parser)
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

void printError(const boost::system::error_code& code)
{
    ERRLOG(logError) << "Relay error: " << code.message();
}

void printHeaders(const RelayPtr& relayPtr)
{
    std::map<std::string, std::string>::const_iterator it;
    for (it = relayPtr->headers().begin(); it != relayPtr->headers().end(); ++it) {
        ERRLOG(logInfo) << it->first << ": " << it->second;
    }
}
