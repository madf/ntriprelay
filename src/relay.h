#ifndef __CASTER_RELAY_H__
#define __CASTER_RELAY_H__

#include "client.h"
#include "server.h"
#include "callbacks.h"

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include <memory>
#include <string>
#include <map>
#include <cstdint>

namespace Caster {

class Relay : public std::enable_shared_from_this<Relay>
{
    public:
        using ErrorCallback = std::function<void (const std::string&)>;

        Relay(boost::asio::io_service& ioService,
              const std::string& srcServer, uint16_t srcPort,
              const std::string& srcMountpoint,
              const std::string& dstServer, uint16_t dstPort,
              const std::string& dstMountpoint);

        void start()
        { initCallbacks(); m_client.start(); m_server.start(); }

        void start(unsigned timeout)
        {
            initCallbacks();
            m_client.start(timeout);
            m_server.start(timeout);
        }

        void setGGA(const std::string& gga) { m_client.setGGA(gga); }
        void setSrcCredentials(const std::string& login,
                               const std::string& password)
        { m_client.setCredentials(login, password); }
        void setDstCredentials(const std::string& login,
                               const std::string& password)
        { m_server.setCredentials(login, password); }

        void setErrorCallback(const ErrorCallback& cb) { m_errorCallback = cb; }
        void setEOFCallback(const EOFCallback& cb) { m_eofCallback = cb; }
        void setHeadersCallback(const HeadersCallback& cb) { m_client.setHeadersCallback(cb); }

        const std::map<std::string, std::string>& headers() const { return m_client.headers(); }

    private:
        Client m_client;
        Server m_server;
        ErrorCallback m_errorCallback;
        EOFCallback m_eofCallback;

        void initCallbacks();
        void clearCallbacks();
        void handleClientError(const boost::system::error_code& ec);
        void handleServerError(const boost::system::error_code& ec);
        void handleError(const std::string& message);
        void handleData(const boost::asio::const_buffers_1& buffers);
        void handleEOF();
};

using RelayPtr = std::shared_ptr<Relay>;

}

#endif
