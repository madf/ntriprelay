#ifndef __CASTER_SERVER_H__
#define __CASTER_SERVER_H__

#include "connection.h"

#include <boost/asio.hpp>

#include <string>
#include <cstdint>

namespace Caster {

class Server : private Connection {
    public:
        Server(boost::asio::io_service& ioService,
               const std::string& server, uint16_t port,
               const std::string& mountpoint);

        using Connection::start;
        using Connection::stop;
        using Connection::setCredentials;
        using Connection::setErrorCallback;
        using Connection::resetErrorCallback;
        using Connection::isActive;

        void send(const boost::asio::const_buffer& buffer);

    private:
        void m_prepareRequest() override;
};

}

#endif
