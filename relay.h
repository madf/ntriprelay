#ifndef __CASTER_RELAY_H__
#define __CASTER_RELAY_H__

#include "client.h"
#include "server.h"
#include "callbacks.h"

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>

#include <string>
#include <map>

namespace Caster {

class Relay : public boost::enable_shared_from_this<Relay>,
              private boost::noncopyable {
    public:
        Relay(boost::asio::io_service& ioService,
              const std::string& srcServer, uint16_t srcPort,
              const std::string& srcMountpoint,
              const std::string& dstServer, uint16_t dstPort,
              const std::string& dstMountpoint);
        ~Relay();

        void start()
        { m_initCallbacks(); m_client.start(); m_server.start(); }
        void start(unsigned timeout)
        {
            m_initCallbacks();
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

        void m_initCallbacks();
        void m_clearCallbacks();
        void m_handleError(const boost::system::error_code& code);
        void m_handleData(const boost::asio::const_buffers_1& buffers);
        void m_handleEOF();
};

typedef boost::shared_ptr<Relay> RelayPtr;

}

#endif
