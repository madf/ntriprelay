#ifndef __CASTER_CONNECTION_H__
#define __CASTER_CONNECTION_H__

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/bind.hpp>

#include "authenticator.h"
#include "callbacks.h"

using boost::asio::ip::tcp;

namespace Caster {

class Connection : private boost::noncopyable {
    public:
        Connection(boost::asio::io_service & ioService,
                   const std::string & server, uint16_t port);

        Connection(boost::asio::io_service & ioService,
                   const std::string & server, uint16_t port,
                   const std::string & mountpoint);
        virtual ~Connection() {}

        void start();
        void start(unsigned timeout);
        void stop() { _shutdown(); }

        void send(const boost::asio::const_buffers_1 & buffers);

        void setCredentials(const std::string & login,
                            const std::string & password);

        void setErrorCallback(const ErrorCallback & cb) { _errorCallback = cb; }
        void setDataCallback(const DataCallback & cb) { _dataCallback = cb; }
        void setEOFCallback(const EOFCallback & cb) { _eofCallback = cb; }
        void setHeadersCallback(const HeadersCallback & cb) { _headersCallback = cb; }

        void resetErrorCallback() { _errorCallback.clear(); }
        void resetDataCallback() { _dataCallback.clear(); }
        void resetEOFCallback() { _eofCallback.clear(); }
        void resetHeadersCallback() { _headersCallback.clear(); }

        const std::map<std::string, std::string> & headers() const { return _headers; }

        bool isActive() const { return _active; }

    protected:
        std::string _server;
        uint16_t _port;
        std::string _uri;
        Authenticator _auth;
        unsigned _timeout;
        std::map<std::string, std::string> _headers;
        tcp::socket _socket;
        boost::asio::deadline_timer _timeouter;
        boost::asio::streambuf _request;

        virtual void _prepareRequest() = 0;

    private:
        tcp::resolver _resolver;
        boost::asio::streambuf _response;
        ErrorCallback _errorCallback;
        DataCallback _dataCallback;
        EOFCallback _eofCallback;
        HeadersCallback _headersCallback;
        bool _chunked;
        bool _active;

        void _handleResolve(const boost::system::error_code & error,
                            tcp::resolver::iterator it);
        void _handleConnect(const boost::system::error_code & error,
                            tcp::resolver::iterator it);
        void _handleWriteRequest(const boost::system::error_code & error);
        void _handleWriteData(const boost::system::error_code & error);
        void _handleReadStatus(const boost::system::error_code & error);
        void _handleReadHeaders(const boost::system::error_code & error);
        void _handleReadData(const boost::system::error_code & error);
        void _handleReadChunkLength(const boost::system::error_code & error);
        void _handleReadChunkData(const boost::system::error_code & error,
                                  size_t size);
        void _handleTimeout();
        void _shutdown();
};

}

#endif
