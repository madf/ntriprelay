#include "server.h"

#include "version.h"

#include <boost/format.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>

#include <iostream>

using Caster::Server;

Server::Server(boost::asio::io_service& ioService,
               const std::string& server, uint16_t port,
               const std::string& mountpoint)
    : Connection(ioService, server, port, mountpoint)
{
}

void Server::send(const boost::asio::const_buffer& buffer)
{
    const std::string dataLength((boost::format("%|x|\r\n") % boost::asio::buffer_size(buffer)).str());
    const boost::array<boost::asio::const_buffer, 3> bufs = {{
        boost::asio::buffer(dataLength),
        boost::asio::buffer(buffer),
        boost::asio::buffer("\r\n", 2)
    }};
    Connection::send(bufs);
}

void Server::prepareRequest()
{
    std::ostream requestStream(&m_request);
    requestStream << "POST " << m_uri << " HTTP/1.1\r\n"
                  << "Host: " << m_server << "\r\n"
                  << "Ntrip-Version: Ntrip/2.0\r\n"
                  << "User-Agent: Boost.Asio NTRIP Server " << version
                  << "\r\n";
    if (m_auth.authenticated())
        requestStream << "Authorization: Basic " << m_auth.basic() << "\r\n";
    requestStream << "Connection: close\r\n"
                  << "Transfer-Encoding: chunked\r\n"
                  << "\r\n";
}
