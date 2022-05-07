#include "connection.h"

#include "error.h"
#include "logger.h"
#include "utils.h"

#include <boost/lexical_cast.hpp>

#define ERRLOG(level) LOG(CerrWriter, level)

using namespace MADF;
using Caster::Connection;

namespace pls = std::placeholders;
namespace bs = boost::system;
namespace ba = boost::asio;

namespace
{

using StringPair = std::pair<std::string, std::string>;

StringPair splitString(const std::string& src, char delimiter)
{
    const size_t pos = src.find_first_of(delimiter);
    if (pos != std::string::npos)
        return std::make_pair(src.substr(0, pos), src.substr(pos + 1));

    return std::make_pair(src, "");
}

StringPair trimStringPair(const StringPair& pair)
{
    const size_t lpos = pair.second.find_first_not_of(" \t");
    const size_t rpos = pair.second.find_last_not_of(" \t\r\n", std::string::npos, 4);
    return std::make_pair(pair.first, pair.second.substr(lpos, rpos - lpos + 1));
}

}

Connection::Connection(ba::io_service& ioService,
                       const std::string& server, uint16_t port)
    : m_server(server),
      m_port(port),
      m_uri("/"),
      m_timeout(0),
      m_socket(ioService),
      m_timeouter(ioService),
      m_resolver(ioService),
      m_response(1024),
      m_chunked(false),
      m_active(false)
{
}

Connection::Connection(ba::io_service& ioService,
                       const std::string& server, uint16_t port,
                       const std::string& mountpoint)
    : m_server(server),
      m_port(port),
      m_timeout(0),
      m_socket(ioService),
      m_timeouter(ioService),
      m_resolver(ioService),
      m_response(1024),
      m_chunked(false),
      m_active(false)
{
    if (mountpoint[0] != '/')
        m_uri = "/";
    m_uri += mountpoint;
}

void Connection::start()
{
    m_resolver.async_resolve(tcp::resolver::query(m_server, boost::lexical_cast<std::string>(m_port)),
                             std::bind(&Connection::handleResolve, this, pls::_1, pls::_2));
    restartTimer();
}

void Connection::start(unsigned timeout)
{
    m_timeout = timeout;
    start();
}

void Connection::setCredentials(const std::string& login,
                                const std::string& password)
{
    m_auth = Authenticator(login, password);
}

void Connection::handleResolve(const bs::error_code& error,
                               tcp::resolver::iterator it)
{
    if (error)
    {
        reportError(error);
        shutdown();
        return;
    }

    if (it == tcp::resolver::iterator())
    {
        reportError(resolveError);
        shutdown();
        return;
    }

    ERRLOG(logDebug) << "Endpoints to connect:";
    for (auto i = it; i != tcp::resolver::iterator(); ++i)
        ERRLOG(logDebug) << i->endpoint();

    restartTimer();
    ERRLOG(logDebug) << "Trying to connect to " << it->endpoint();
    m_socket.async_connect(*it, std::bind(&Connection::handleConnect, this, pls::_1, it));
}

void Connection::handleConnect(const bs::error_code& error,
                               tcp::resolver::iterator it)
{
    if (error)
    {
        ERRLOG(logDebug) << "Error connecting to " << it->endpoint() << ": " << error.message();
        ++it;
        ERRLOG(logDebug) << "Trying to connect to " << it->endpoint();
        if (it == tcp::resolver::iterator())
        {
            reportError(error);
            shutdown();
            return;
        }
        restartTimer();
        m_socket.close();
        m_socket.async_connect(*it, std::bind(&Connection::handleConnect, this, pls::_1, it));
        return;
    }

    ERRLOG(logDebug) << "Successfully connected to " << m_socket.remote_endpoint();

    restartTimer();
    prepareRequest();
    ba::async_write(m_socket, m_request, ba::transfer_all(), std::bind(&Connection::handleWriteRequest, this, pls::_1));
}

void Connection::handleWriteRequest(const bs::error_code& error)
{
    if (error)
    {
        if (error != ba::error::operation_aborted)
        {
            reportError(error);
            shutdown();
        }
        return;
    }

    restartTimer();
    ba::async_read_until(m_socket, m_response, "\r\n", std::bind(&Connection::handleReadStatus, this, pls::_1));
}

void Connection::handleWriteData(const bs::error_code& error)
{
    if (error)
    {
        if (error != ba::error::operation_aborted)
        {
            reportError(error);
            shutdown();
        }
        return;
    }

    restartTimer();
}

void Connection::handleReadStatus(const bs::error_code& error)
{
    restartTimer();
    if (!error) {
        std::istream statusStream(&m_response);
        std::string proto;
        statusStream >> proto;
        unsigned code;
        statusStream >> code;
        std::string message;
        std::getline(statusStream, message);
        if (code != 200) {
            ERRLOG(logError) << "Invalid status string:\n"
                          << proto << " " << code << " " << message;
            reportError(invalidStatus);
            shutdown();
            return;
        }
        if (proto == "ICY") {
            m_active = true;
            ba::async_read(
                m_socket,
                m_response,
                std::bind(
                    &Connection::handleReadData,
                    this,
                    pls::_1
                )
            );
        } else {
            ba::async_read_until(
                m_socket,
                m_response,
                "\r\n\r\n",
                std::bind(
                    &Connection::handleReadHeaders,
                    this,
                    pls::_1
                )
            );
        }
    } else if (error != ba::error::operation_aborted) {
        reportError(error);
        shutdown();
    }
}

void Connection::handleReadHeaders(const bs::error_code& error)
{
    restartTimer();
    if (!error) {
        std::istream headersStream(&m_response);
        std::string header;
        while (std::getline(headersStream, header) && header != "\r") {
            const StringPair pair(trimStringPair(splitString(header, ':')));
            m_headers.insert(pair);
            if (pair.first == "Transfer-Encoding" &&
                pair.second == "chunked") {
                ERRLOG(logDebug) << "Transfer-Encoding: chunked";
                m_chunked = true;
            }
        }
        if (m_headersCallback)
            m_headersCallback();
        m_active = true;
        if (m_chunked) {
            ba::async_read_until(
                m_socket,
                m_response,
                "\r\n",
                std::bind(
                    &Connection::handleReadChunkLength,
                    this,
                    pls::_1
                )
            );
        } else {
            ba::async_read(
                m_socket,
                m_response,
                std::bind(
                    &Connection::handleReadData,
                    this,
                    pls::_1
                )
            );
        }
    } else if (error != ba::error::operation_aborted) {
        reportError(error);
        shutdown();
    }
}

void Connection::handleReadData(const bs::error_code& error)
{
    restartTimer();

    if (m_response.size() > 0) {
        if (m_dataCallback)
            m_dataCallback(m_response.data());
        m_response.consume(m_response.size());
    }

    if (!error) {
        ba::async_read(
            m_socket,
            m_response,
            ba::transfer_at_least(1),
            std::bind(
                &Connection::handleReadData,
                this,
                pls::_1
            )
        );
    } else if (error == ba::error::eof) {
        if (m_eofCallback)
            m_eofCallback();
        shutdown();
    } else if (error != ba::error::operation_aborted) {
        reportError(error);
        shutdown();
    }
}

void Connection::handleReadChunkLength(const bs::error_code& error)
{
    restartTimer();

    if (!error) {
        size_t length = 0;
        m_response.consume(parseChunkLength(m_response.data(), length));
        if (length == 0) {
            if (m_eofCallback)
                m_eofCallback();
            shutdown();
        } else {
            if (m_response.size() < length + 2) {
                ba::async_read(
                    m_socket,
                    m_response,
                    ba::transfer_at_least(length + 2 - m_response.size()),
                    std::bind(
                        &Connection::handleReadChunkData,
                        this,
                        pls::_1,
                        length
                    )
                );
            } else {
                handleReadChunkData(bs::error_code(), length);
            }
        }
    } else if (error == ba::error::eof) {
        if (m_eofCallback)
            m_eofCallback();
        shutdown();
    } else if (error != ba::error::operation_aborted) {
        reportError(error);
        shutdown();
    }
}

void Connection::handleReadChunkData(const bs::error_code& error,
                                       size_t size)
{
    restartTimer();

    if (size > 0) {
        if (size > m_response.size()) {
            if (m_dataCallback)
                m_dataCallback(m_response.data());
        } else {
            if (m_dataCallback)
                m_dataCallback(buffer(m_response.data(), size));
        }
    }
    if (!error) {
        if (size > m_response.size()) {
            const size_t remainder = size + 2 - m_response.size();
            m_response.consume(m_response.size());
            ba::async_read(
                m_socket,
                m_response,
                ba::transfer_at_least(remainder),
                std::bind(
                    &Connection::handleReadChunkData,
                    this,
                    pls::_1,
                    remainder - 2
                )
            );
        } else {
            m_response.consume(size + 2);
            ba::async_read_until(
                m_socket,
                m_response,
                "\r\n",
                std::bind(
                    &Connection::handleReadChunkLength,
                    this,
                    pls::_1
                )
            );
        }
    } else if (error == ba::error::eof) {
        if (m_eofCallback)
            m_eofCallback();
        shutdown();
    } else if (error != ba::error::operation_aborted) {
        reportError(error);
        shutdown();
    }
}

void Connection::shutdown()
{
    m_active = false;
    if (!m_socket.is_open())
        return;
    ERRLOG(logDebug) << "Connection::shutdown()";
    bs::error_code ec;
    m_socket.shutdown(tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

void Connection::handleTimeout(const bs::error_code& ec)
{
    if (ec == ba::error::operation_aborted)
        return;
    if (!m_socket.is_open())
        return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (m_timeouter.expires_at() > std::chrono::steady_clock::now())
        m_timeouter.async_wait(std::bind(&Connection::handleTimeout, this, pls::_1));

    ERRLOG(logInfo) << "Connection timeout detected, shutting it down" << std::endl;
    reportError(connectionTimeout);
    shutdown();
}

void Connection::restartTimer()
{
    if (m_timeout > 0)
        return;

    m_timeouter.expires_from_now(std::chrono::seconds(m_timeout));
    m_timeouter.async_wait(std::bind(&Connection::handleTimeout, this, pls::_1));
}

void Connection::reportError(const bs::error_code& ec)
{
    if (m_errorCallback)
        m_errorCallback(ec);
}

void Connection::reportError(int val)
{
    reportError(boost::system::error_code(val, Category::getInstance()));
}
