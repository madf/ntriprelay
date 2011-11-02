#ifndef __CASTER_RELAY_H__
#define __CASTER_RELAY_H__

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>

#include "client.h"
#include "server.h"
#include "callbacks.h"

namespace Caster {

class Relay : public boost::enable_shared_from_this<Relay>,
              private boost::noncopyable {
    public:
        Relay(boost::asio::io_service & ioService,
              const std::string & srcServer, uint16_t srcPort,
              const std::string & srcMountpoint,
              const std::string & dstServer, uint16_t dstPort,
              const std::string & dstMountpoint);
        ~Relay();

        void start()
        { _initCallbacks(); _client.start(); _server.start(); }
        void start(unsigned timeout)
        {
            _initCallbacks();
            _client.start(timeout);
            _server.start(timeout);
        }

        void setGGA(const std::string & gga) { _client.setGGA(gga); }
        void setSrcCredentials(const std::string & login,
                               const std::string & password)
        { _client.setCredentials(login, password); }
        void setDstCredentials(const std::string & login,
                               const std::string & password)
        { _server.setCredentials(login, password); }

        void setErrorCallback(const ErrorCallback & cb) { _errorCallback = cb; }
        void setEOFCallback(const EOFCallback & cb) { _eofCallback = cb; }
        void setHeadersCallback(const HeadersCallback & cb) { _client.setHeadersCallback(cb); }

        const std::map<std::string, std::string> & headers() const { return _client.headers(); }

    private:
        Client _client;
        Server _server;
        ErrorCallback _errorCallback;
        EOFCallback _eofCallback;

        void _initCallbacks();
        void _clearCallbacks();
        void _handleError(const boost::system::error_code & code);
        void _handleData(const boost::array<char, 2048> & data,
                         size_t amount);
        void _handleEOF();
};

typedef boost::shared_ptr<Relay> RelayPtr;

}

#endif
