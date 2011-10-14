#include <boost/lexical_cast.hpp>

#include "error.h"
#include "logger.h"

#include "connection.h"

#define ERRLOG(level) LOG(CerrWriter, level)

using namespace MADF;
using Caster::Connection;
using namespace boost::asio;

Connection::Connection(boost::asio::io_service & ioService,
                       const std::string & server, uint16_t port)
    : _server(server),
      _port(port),
      _uri("/"),
      _auth(),
      _timeout(0),
      _headers(),
      _socket(ioService),
      _timeouter(ioService),
      _request(),
      _resolver(ioService),
      _response(),
      _buffer(),
      _errorCallback(),
      _dataCallback(),
      _eofCallback(),
      _active(false)
{
}

Connection::Connection(boost::asio::io_service & ioService,
                       const std::string & server, uint16_t port,
                       const std::string & mountpoint)
    : _server(server),
      _port(port),
      _uri(),
      _auth(),
      _timeout(0),
      _headers(),
      _socket(ioService),
      _timeouter(ioService),
      _request(),
      _resolver(ioService),
      _response(),
      _buffer(),
      _errorCallback(),
      _dataCallback(),
      _eofCallback(),
      _active(false)
{
    if (mountpoint[0] != '/')
        _uri = "/";
    _uri += mountpoint;
}

void Connection::start()
{
    tcp::resolver::query query(_server, boost::lexical_cast<std::string>(_port));
    _resolver.async_resolve(
            query,
            boost::bind(
                &Connection::_handleResolve,
                this,
                placeholders::error,
                placeholders::iterator
            )
    );
}

void Connection::start(unsigned timeout)
{
    _timeout = timeout;
    start();
    _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    _timeouter.async_wait(
            boost::bind(
                &Connection::_handleTimeout,
                this
            )
    );
}

void Connection::setCredentials(const std::string & login,
                            const std::string & password)
{
    _auth.setLogin(login);
    _auth.setPassword(password);
}

void Connection::_handleResolve(const boost::system::error_code & error,
                                tcp::resolver::iterator it)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        if (it != tcp::resolver::iterator()) {
            _socket.async_connect(
                *it,
                boost::bind(
                    &Connection::_handleConnect,
                    this,
                    placeholders::error,
                    it
                )
            );
        } else {
            if (!_errorCallback.empty())
                _errorCallback(
                    boost::system::error_code(
                        resolveError,
                        CasterCategory::getInstance()
                    )
                );
            _shutdown();
        }
    } else {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Connection::_handleConnect(const boost::system::error_code & error,
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
                &Connection::_handleWriteRequest,
                this,
                placeholders::error
            )
        );
    } else {
        ++it;
        if (it != tcp::resolver::iterator()) {
            _socket.async_connect(
                *it,
                boost::bind(
                    &Connection::_handleConnect,
                    this,
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

void Connection::_handleWriteRequest(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        async_read_until(
            _socket,
            _response,
            "\r\n",
            boost::bind(
                &Connection::_handleReadStatus,
                this,
                placeholders::error
            )
        );
    } else if (error != error::operation_aborted) {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Connection::_handleWriteData(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (error && error != error::operation_aborted) {
        if (!_errorCallback.empty())
            _errorCallback(error);
        _shutdown();
    }
}

void Connection::_handleReadStatus(const boost::system::error_code & error)
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
            if (!_errorCallback.empty())
                _errorCallback(
                    boost::system::error_code(
                        invalidStatus,
                        CasterCategory::getInstance()
                    )
                );
            _shutdown();
            return;
        }
        if (proto == "ICY") {
            _active = true;
            _socket.async_read_some(
                buffer(_buffer),
                boost::bind(
                    &Connection::_handleReadData,
                    this,
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
                    &Connection::_handleReadHeaders,
                    this,
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

void Connection::_handleReadHeaders(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));
    if (!error) {
        std::istream headersStream(&_response);
        std::string header;
        while (std::getline(headersStream, header) && header != "\r") {
            _headers.push_back(header);
        }
        _active = true;
        _socket.async_read_some(
            buffer(_buffer),
            boost::bind(
                &Connection::_handleReadData,
                this,
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

void Connection::_handleReadData(const boost::system::error_code & error,
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
                &Connection::_handleReadData,
                this,
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

void Connection::_shutdown()
{
    _active = false;
    if (!_socket.is_open())
        return;
    boost::system::error_code ec;
    _socket.shutdown(tcp::socket::shutdown_both, ec);
    _socket.close(ec);
}

void Connection::_handleTimeout()
{
    if (_socket.is_open()) {
        // Check whether the deadline has passed. We compare the deadline against
        // the current time since a new asynchronous operation may have moved the
        // deadline before this actor had a chance to run.
        if (_timeouter.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
            ERRLOG(logInfo) << "Connection timeout detected, shutting it down" << std::endl;
            if (!_errorCallback.empty())
                _errorCallback(
                    boost::system::error_code(
                        connectionTimeout,
                        CasterCategory::getInstance()
                    )
                );
            _shutdown();
            return;
        }
        // Reschedule timeout
        _timeouter.async_wait(
                boost::bind(
                    &Connection::_handleTimeout,
                    this
                )
        );
    }
}
