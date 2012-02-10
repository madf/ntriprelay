#include <iostream>

#include "version.h"

#include "client.h"

using Caster::Client;
using namespace boost::asio;

Client::Client(io_service & ioService,
               const std::string & server, uint16_t port)
    : Connection(ioService, server, port),
      m_gga()
{
}

Client::Client(io_service & ioService,
               const std::string & server, uint16_t port,
               const std::string & mountpoint)
    : Connection(ioService, server, port, mountpoint),
      m_gga()
{
}

void Client::m_prepareRequest()
{
    std::ostream requestStream(&m_request);
    requestStream << "GET " << m_uri << " HTTP/1.1\r\n"
                  << "Host: " << m_server << "\r\n"
                  << "Ntrip-Version: Ntrip/2.0\r\n"
                  << "User-Agent: Boost.Asio NTRIP Client " << version
                  << "\r\n";
    if (m_auth.authenticated())
        requestStream << "Authorization: Basic " << m_auth.basic() << "\r\n";
    if (!m_gga.empty())
        requestStream << "Ntrip-GGA: " << m_gga << "\r\n";
    requestStream << "Connection: close\r\n"
                  << "\r\n";
    if (!m_gga.empty())
        requestStream << m_gga << "\r\n"; // Version 1.0
}
