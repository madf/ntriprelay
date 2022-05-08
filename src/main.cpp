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

namespace
{

void configureLogger(const Settings& settings)
{
    if (settings.isDebug())
    {
        switch (settings.verbosity())
        {
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
    }
    else
    {
        switch (settings.verbosity())
        {
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
    for (const auto& kv : relayPtr->headers())
        ERRLOG(logInfo) << kv.first << ": " << kv.second;
}

}

int main(int argc, char* argv[])
{
    SettingsParser sParser;

    try
    {
        sParser.init(argc, argv);
    }
    catch (const Error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error!" << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }

    const auto& settings = sParser.settings();

    if (settings.isHelp())
    {
        sParser.printHelp();
        return 0;
    }

    if (settings.isVersion())
    {
        std::cout << "Boost NTRIP Relay " << version << std::endl;
        return 0;
    }

    if (settings.sourceServer().empty())
    {
        std::cerr << "You must specify source server location" << std::endl;
        return -1;
    }

    if (settings.destinationServer().empty())
    {
        std::cerr << "You must specify destination server location" << std::endl;
        return -1;
    }

    configureLogger(settings);

    if (settings.isDebug())
    {
        std::cout << "Settings dump:\n"
                  << "\t- connection timeout: " << settings.connectionTimeout() << "\n"
                  << "\t- debug: " << (settings.isDebug() ? "yes" : "no") << "\n"
                  << "\t- destination login: " << settings.destinationLogin() << "\n"
                  << "\t- destination mountpoint: " << settings.destinationMountpoint() << "\n"
                  << "\t- destination password: " << settings.destinationPassword() << "\n"
                  << "\t- destination port: " << settings.destinationPort() << "\n"
                  << "\t- destination server: " << settings.destinationServer() << "\n"
                  << "\t- GGA: " << settings.gga() << "\n"
                  << "\t- help: " << (settings.isHelp() ? "yes" : "no") << "\n"
                  << "\t- source login: " << settings.sourceLogin() << "\n"
                  << "\t- source mountpoint: " << settings.sourceMountpoint() << "\n"
                  << "\t- source password: " << settings.sourcePassword() << "\n"
                  << "\t- source port: " << settings.sourcePort() << "\n"
                  << "\t- source server: " << settings.sourceServer() << "\n"
                  << "\t- verbosity level: " << settings.verbosity() << "\n"
                  << "\t- version: " << (settings.isVersion() ? "yes" : "no") << std::endl;
    }

    try
    {
        boost::asio::io_service ioService;
        auto relay = std::make_shared<Relay>(ioService,
                                             settings.sourceServer(),
                                             settings.sourcePort(),
                                             settings.sourceMountpoint(),
                                             settings.destinationServer(),
                                             settings.destinationPort(),
                                             settings.destinationMountpoint());

        relay->setErrorCallback(printError);

        relay->setHeadersCallback(std::bind(printHeaders, relay));

        if (!settings.sourceLogin().empty() ||
            !settings.sourcePassword().empty())
        {
            relay->setSrcCredentials(settings.sourceLogin(),
                                     settings.sourcePassword());
        }

        if (!settings.destinationLogin().empty() ||
            !settings.destinationPassword().empty())
        {
            relay->setDstCredentials(settings.destinationLogin(),
                                     settings.destinationPassword());
        }

        if (!settings.gga().empty())
            relay->setGGA(settings.gga());

        ERRLOG(logDebug) << "Before starting...";

        relay->start(settings.connectionTimeout());

        ERRLOG(logDebug) << "Starting...";

        ioService.run();

        ERRLOG(logDebug) << "Stopping...";
    }
    catch (const Error& e)
    {
        ERRLOG(logError) << "Relay error: " << e.what();
    }
    catch (const std::exception& e)
    {
        ERRLOG(logFatal) << "System error: " << e.what();
        return -1;
    }

    return 0;
}
