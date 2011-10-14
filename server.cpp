#include <iostream>

#include <boost/format.hpp>

#include "version.h"

#include "server.h"

using Caster::Server;

Server::Server(boost::asio::io_service & ioService,
               const std::string & server, uint16_t port,
               const std::string mountpoint)
    : Connection(ioService, server, port, mountpoint)
{
}

void Server::send(const boost::asio::const_buffer & buffer)
{
    std::string dataLength((boost::format("%|x|\r\n") % boost::asio::buffer_size(buffer)).str());
    boost::array<boost::asio::const_buffer, 3> bufs = {{
        boost::asio::buffer(dataLength),
        boost::asio::buffer(buffer),
        boost::asio::buffer("\r\n", 2)
    }};
    Connection::send(bufs);
}

void Server::_prepareRequest()
{
    std::ostream requestStream(&_request);
    requestStream << "POST " << _uri << " HTTP/1.1\r\n"
                  << "Host: " << _server << "\r\n"
                  << "Ntrip-Version: Ntrip/2.0\r\n"
                  << "User-Agent: Boost.Asio NTRIP Server " << version
                  << "\r\n";
    if (_auth.authenticated())
        requestStream << "Authorization: Basic " << _auth.basic() << "\r\n";
    requestStream << "Connection: close\r\n"
                  << "Transfer-Encoding: chunked\r\n"
                  << "\r\n";
}
