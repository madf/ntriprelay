#include <boost/lexical_cast.hpp>

#include "error.h"
#include "logger.h"
#include "utils.h"

#include "connection.h"

#define ERRLOG(level) LOG(CerrWriter, level)

using namespace MADF;
using Caster::Connection;
using Caster::parseChunkLength;
using namespace boost::asio;

namespace {

typedef std::pair<std::string, std::string> StringPair;

StringPair splitString(const std::string & src, char delimiter);
StringPair trimStringPair(const StringPair & pair);

}

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
      _response(1024),
      _errorCallback(),
      _dataCallback(),
      _eofCallback(),
      _headersCallback(),
      _chunked(false),
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
      _response(1024),
      _errorCallback(),
      _dataCallback(),
      _eofCallback(),
      _chunked(false),
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
            transfer_all(),
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
            async_read(
                _socket,
                _response,
                boost::bind(
                    &Connection::_handleReadData,
                    this,
                    placeholders::error
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
            const StringPair pair(trimStringPair(splitString(header, ':')));
            _headers.insert(pair);
            if (pair.first == "Transfer-Encoding" &&
                pair.second == "chunked") {
                ERRLOG(logDebug) << "Transfer-Encoding: chunked";
                _chunked = true;
            }
        }
        if (!_headersCallback.empty())
            _headersCallback();
        _active = true;
        if (_chunked) {
            async_read_until(
                _socket,
                _response,
                "\r\n",
                boost::bind(
                    &Connection::_handleReadChunkLength,
                    this,
                    placeholders::error
                )
            );
        } else {
            async_read(
                _socket,
                _response,
                boost::bind(
                    &Connection::_handleReadData,
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

void Connection::_handleReadData(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));

    if (_response.size()) {
        if (!_dataCallback.empty())
            _dataCallback(_response.data());
        _response.consume(_response.size());
    }

    if (!error) {
        async_read(
            _socket,
            _response,
            transfer_at_least(1),
            boost::bind(
                &Connection::_handleReadData,
                this,
                placeholders::error
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

void Connection::_handleReadChunkLength(const boost::system::error_code & error)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));

    if (!error) {
        size_t length = 0;
        _response.consume(parseChunkLength(_response.data(), length));
        if (length == 0) {
            if (!_eofCallback.empty())
                _eofCallback();
            _shutdown();
        } else {
            if (_response.size() < length + 2) {
                async_read(
                    _socket,
                    _response,
                    transfer_at_least(length + 2 - _response.size()),
                    boost::bind(
                        &Connection::_handleReadChunkData,
                        this,
                        placeholders::error,
                        length
                    )
                );
            } else {
                _handleReadChunkData(boost::system::error_code(), length);
            }
        }
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

void Connection::_handleReadChunkData(const boost::system::error_code & error,
                                      size_t size)
{
    if (_timeout)
        _timeouter.expires_from_now(boost::posix_time::seconds(_timeout));

    if (size > 0) {
        if (size > _response.size()) {
            if (!_dataCallback.empty())
                _dataCallback(_response.data());
        } else {
            if (!_dataCallback.empty())
                _dataCallback(buffer(_response.data(), size));
        }
    }
    if (!error) {
        if (size > _response.size()) {
            const size_t remainder = size + 2 - _response.size();
            _response.consume(_response.size());
            async_read(
                _socket,
                _response,
                transfer_at_least(remainder),
                boost::bind(
                    &Connection::_handleReadChunkData,
                    this,
                    placeholders::error,
                    remainder - 2
                )
            );
        } else {
            _response.consume(size + 2);
            async_read_until(
                _socket,
                _response,
                "\r\n",
                boost::bind(
                    &Connection::_handleReadChunkLength,
                    this,
                    placeholders::error
                )
            );
        }
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

namespace {

inline
StringPair splitString(const std::string & src, const char delimiter)
{
    const size_t pos = src.find_first_of(delimiter);
    if (pos != std::string::npos) {
        return std::make_pair(src.substr(0, pos), src.substr(pos + 1));
    } else {
        return std::make_pair(src, "");
    }
}

inline
StringPair trimStringPair(const StringPair & pair)
{
    const size_t lpos = pair.second.find_first_not_of(" \t");
    const size_t rpos = pair.second.find_last_not_of(" \t\r\n", std::string::npos, 4);
    return std::make_pair(pair.first, pair.second.substr(lpos, rpos - lpos + 1));
}

}
