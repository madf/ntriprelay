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

        void start() { Connection::start(); }
        void start(unsigned timeout) { Connection::start(timeout); }
        void stop() { Connection::stop(); }

        void setCredentials(const std::string & login,
                            const std::string & password)
        { Connection::setCredentials(login, password); }

        void send(const boost::asio::const_buffer & buffer)
        { Connection::send(buffer); }

        void setErrorCallback(const ErrorCallback & cb)
        { Connection::setErrorCallback(cb); }

        bool isActive() const { return Connection::isActive(); }

    private:
        void _prepareRequest();
};

typedef boost::shared_ptr<Server> ServerPtr;

}

#endif
