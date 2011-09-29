#ifndef __CASTER_CONNECTION_H__
#define __CASTER_CONNECTION_H__

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/cstdint.hpp>
#include <boost/system/error_code.hpp>

#include "authenticator.h"

using boost::asio::ip::tcp;

namespace Caster {

class NTRIPRequest;

typedef boost::function<void (const boost::system::error_code & code)> ErrorCallback;
typedef boost::function<void (const boost::array<char, 1024> & data,
                              size_t amount)> DataCallback;
typedef boost::function<void ()> EOFCallback;

class Client : public boost::enable_shared_from_this<Client>,
               private boost::noncopyable {
    public:
        enum { Basic, Digest };

        Client(boost::asio::io_service & ioService,
               const std::string & server, uint16_t port);

        Client(boost::asio::io_service & ioService,
               const std::string & server, uint16_t port,
               const std::string & mountpoint);

        void start();
        void start(unsigned timeout);

        void setGGA(const std::string & gga) { _gga = gga; }
        void setCredentials(const std::string & login,
                            const std::string & password);

        void setErrorCallback(const ErrorCallback & cb) { _errorCallback = cb; }
        void setDataCallback(const DataCallback & cb) { _dataCallback = cb; }
        void setEOFCallback(const EOFCallback & cb) { _eofCallback = cb; }

        const std::vector<std::string> & headers() const { return _headers; }

    private:
        boost::asio::io_service & _ioService;
        tcp::socket _socket;
        tcp::resolver _resolver;
        boost::asio::deadline_timer _timeouter;
        std::string _server;
        uint16_t _port;
        std::string _uri;
        std::string _gga;
        Authenticator _auth;
        unsigned _timeout;
        boost::asio::streambuf _request;
        boost::asio::streambuf _response;
        boost::array<char, 1024> _buffer;
        std::vector<std::string> _headers;
        ErrorCallback _errorCallback;
        DataCallback  _dataCallback;
        EOFCallback _eofCallback;

        void _handleResolve(const boost::system::error_code & error,
                            tcp::resolver::iterator it);
        void _handleConnect(const boost::system::error_code & error,
                            tcp::resolver::iterator it);
        void _handleWrite(const boost::system::error_code & error);
        void _handleReadStatus(const boost::system::error_code & error);
        void _handleReadHeaders(const boost::system::error_code & error);
        void _handleReadData(const boost::system::error_code & error,
                             size_t amount);

        void _handleTimeout();
        void _shutdown();
        void _prepareRequest();
};

typedef boost::shared_ptr<Client> ClientPtr;

}

#endif
