#include <iostream>

#include "version.h"

#include "client.h"

using Caster::Client;
using namespace boost::asio;

Client::Client(io_service & ioService,
               const std::string & server, uint16_t port)
    : Connection(ioService, server, port),
      _gga()
{
}

Client::Client(io_service & ioService,
               const std::string & server, uint16_t port,
               const std::string & mountpoint)
    : Connection(ioService, server, port, mountpoint),
      _gga()
{
}

void Client::_prepareRequest()
{
    std::ostream requestStream(&_request);
    requestStream << "GET " << _uri << " HTTP/1.1\r\n"
                  << "Host: " << _server << "\r\n"
                  << "Ntrip-Version: Ntrip/2.0\r\n"
                  << "User-Agent: Boost.Asio NTRIP Client " << version
                  << "\r\n";
    if (_auth.authenticated())
        requestStream << "Authorization: Basic " << _auth.basic() << "\r\n";
    if (!_gga.empty())
        requestStream << "Ntrip-GGA: " << _gga << "\r\n";
    requestStream << "Connection: close\r\n"
                  << "\r\n";
    if (!_gga.empty())
        requestStream << _gga << "\r\n"; // Version 1.0
}
