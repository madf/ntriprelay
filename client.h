#ifndef __CASTER_CLIENT_H__
#define __CASTER_CLIENT_H__

#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>

#include "authenticator.h"
#include "connection.h"

namespace Caster {

class NTRIPRequest;

class Client : public boost::enable_shared_from_this<Client>,
               public Connection {
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

typedef boost::shared_ptr<Client> ClientPtr;

}

#endif
