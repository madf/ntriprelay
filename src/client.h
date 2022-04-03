#ifndef __CASTER_CLIENT_H__
#define __CASTER_CLIENT_H__

#include "authenticator.h"
#include "connection.h"

#include <boost/asio.hpp>

#include <string>
#include <cstdint>

namespace Caster {

class NTRIPRequest;

class Client : public Connection {
    public:
        Client(boost::asio::io_service& ioService,
               const std::string& server, uint16_t port);

        Client(boost::asio::io_service& ioService,
               const std::string& server, uint16_t port,
               const std::string& mountpoint);

        void setGGA(const std::string& gga) { m_gga = gga; }

    private:
        std::string m_gga;

        void m_prepareRequest() override;
};

}

#endif
