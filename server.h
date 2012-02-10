#ifndef __CASTER_SERVER_H__
#define __CASTER_SERVER_H__

#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>

#include "connection.h"

namespace Caster {

class Server : public boost::enable_shared_from_this<Server>,
               private Connection {
    public:
        Server(boost::asio::io_service & ioService,
               const std::string & server, uint16_t port,
               const std::string mountpoint);

        using Connection::start;
        using Connection::stop;
        using Connection::setCredentials;
        using Connection::setErrorCallback;
        using Connection::resetErrorCallback;
        using Connection::isActive;

        void send(const boost::asio::const_buffer & buffer);

    private:
        void m_prepareRequest();
};

typedef boost::shared_ptr<Server> ServerPtr;

}

#endif
