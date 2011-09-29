#include <iostream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "client.h"
#include "version.h"
#include "error.h"
#include "logger.h"

#define ERRLOG(level) LOG(CerrWriter, level)

using namespace MADF;
using Caster::Client;
using namespace boost::asio;

Client::Client(io_service & ioService,
               const std::string & server, uint16_t port)
    : _ioService(ioService),
      _socket(ioService),
      _resolver(ioService),
      _timeouter(ioService),
      _server(server),
      _port(port),
      _uri("/"),
      _gga(),
      _auth(),
      _timeout(0),
      _request(),
      _response(),
      _buffer(),
      _headers(),
      _errorCallback(),
      _dataCallback()
{
}

Client::Client(io_service & ioService,
               const std::string & server, uint16_t port,
               const std::string & mountpoint)
    : _ioService(ioService),
      _socket(ioService),
      _resolver(ioService),
      _timeouter(ioService),
      _server(server),
      _port(port),
      _uri(),
      _gga(),
      _auth(),
      _timeout(0),
      _request(),
      _response(),
      _buffer(),
      _headers(),
      _errorCallback(),
      _dataCallback()
{
    if (mountpoint[0] != '/')
        _uri = "/";
    _uri += mountpoint;
}

void Client::start()
{
    tcp::resolver::query query(_server, boost::lexical_cast<std::string>(_port));
    _resolver.async_resolve(
            query,
            boost::bind(
                &Client::_handleResolve,
                shared_from_this(),
                placeholders::error,
                placeholders::iterator
            )
    );
}

void Client::start(unsigned timeout)
{
    _timeout = timeout;
    start();
    _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    _timeouter.async_wait(
            boost::bind(
                &Client::_handleTimeout,
                shared_from_this()
            )
    );
}

void Client::setCredentials(const std::string & login,
                            const std::string & password)
{
    _auth.setLogin(login);
    _auth.setPassword(password);
}

void Client::_handleResolve(const boost::system::error_code & error,
                            tcp::resolver::iterator it)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        if (it != tcp::resolver::iterator()) {
            _socket.async_connect(
                *it,
                boost::bind(
                    &Client::_handleConnect,
                    shared_from_this(),
                    placeholders::error,
                    it
                )
            );
        } else {
            throw CasterError("Failed to resolve supplied address");
        }
    } else {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Client::_handleConnect(const boost::system::error_code & error,
                            tcp::resolver::iterator it)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        _prepareRequest();
        async_write(
            _socket,
            _request,
            boost::bind(
                &Client::_handleWrite,
                shared_from_this(),
                placeholders::error
            )
        );
    } else {
        ++it;
        if (it != tcp::resolver::iterator()) {
            _socket.async_connect(
                *it,
                boost::bind(
                    &Client::_handleConnect,
                    shared_from_this(),
                    placeholders::error,
                    it
                )
            );
        } else {
            if (!_errorCallback.empty())
                _errorCallback(error);
            _shutdown();
        }
    }
}

void Client::_handleWrite(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        async_read_until(
            _socket,
            _response,
            "\r\n",
            boost::bind(
                &Client::_handleReadStatus,
                shared_from_this(),
                placeholders::error
            )
        );
    } else if (error != error::operation_aborted) {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Client::_handleReadStatus(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        std::istream statusStream(&_response);
        std::string proto;
        statusStream >> proto;
        unsigned code;
        statusStream >> code;
        std::string message;
        std::getline(statusStream, message);
        if (code != 200) {
            ERRLOG(logError) << "Invalid status string:\n"
                          << proto << " " << code << " " << message;
            _shutdown();
            return;
        }
        if (proto == "ICY") {
            _socket.async_read_some(
                buffer(_buffer),
                boost::bind(
                    &Client::_handleReadData,
                    shared_from_this(),
                    placeholders::error,
                    placeholders::bytes_transferred
                )
            );
        } else {
            async_read_until(
                _socket,
                _response,
                "\r\n\r\n",
                boost::bind(
                    &Client::_handleReadHeaders,
                    shared_from_this(),
                    placeholders::error
                )
            );
        }
    } else if (error != error::operation_aborted) {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Client::_handleReadHeaders(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        std::istream headersStream(&_response);
        std::string header;
        while (std::getline(headersStream, header) && header != "\r") {
            _headers.push_back(header);
        }
        _socket.async_read_some(
            buffer(_buffer),
            boost::bind(
                &Client::_handleReadData,
                shared_from_this(),
                placeholders::error,
                placeholders::bytes_transferred
            )
        );
    } else if (error != error::operation_aborted) {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Client::_handleReadData(const boost::system::error_code & error,
                             size_t amount)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (amount) {
        if (!_dataCallback.empty())
            _dataCallback(_buffer, amount);
    }
    if (!error) {
        _socket.async_read_some(
            buffer(_buffer),
            boost::bind(
                &Client::_handleReadData,
                shared_from_this(),
                placeholders::error,
                placeholders::bytes_transferred
            )
        );
    } else if (error == error::eof) {
        if (!_eofCallback.empty())
            _eofCallback();
        _shutdown();
    } else if (error != error::operation_aborted) {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Client::_prepareRequest()
{
    std::ostream requestStream(&_request);
    requestStream << "GET " << _uri << " HTTP/1.1\r\n"
                  << "Host: " << _server << "\r\n"
                  << "Ntrip-Version: Ntrip/2.0\r\n"
                  << "User-Agent: Boost.Asio NTRIP Client " << version
                  << "\r\n";
    if (_auth.authenticated())
        requestStream << "Authorization: Basic " << _auth.basic() << "\r\n";
    if (!_gga.empty())
        requestStream << "Ntrip-GGA: " << _gga << "\r\n";
    requestStream << "Connection: close\r\n"
                  << "\r\n";
    if (!_gga.empty())
        requestStream << _gga << "\r\n"; // Version 1.0
}

void Client::_shutdown()
{
    boost::system::error_code ec;
    _socket.shutdown(tcp::socket::shutdown_both, ec);
    _socket.close();
}

void Client::_handleTimeout()
{
    if (_socket.is_open()) {
        // Check whether the deadline has passed. We compare the deadline against
        // the current time since a new asynchronous operation may have moved the
        // deadline before this actor had a chance to run.
        if (_timeouter.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
            ERRLOG(logInfo) << "Connection timeout detected, shutting it down" << std::endl;
            _shutdown();
            return;
        }
        // Reschedule timeout
        _timeouter.async_wait(
                boost::bind(
                    &Client::_handleTimeout,
                    shared_from_this()
                )
        );
    }
}
