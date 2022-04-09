#ifndef __CASTER_CONNECTION_H__
#define __CASTER_CONNECTION_H__

#include "authenticator.h"
#include "callbacks.h"

#include <boost/asio.hpp>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>

namespace Caster
{

class Connection
{
    public:
        Connection(boost::asio::io_service& ioService,
                   const std::string& server, uint16_t port);

        Connection(boost::asio::io_service& ioService,
                   const std::string& server, uint16_t port,
                   const std::string& mountpoint);
        virtual ~Connection() = default;

        void start();
        void start(unsigned timeout);
        void stop() { shutdown(); }

        template <typename ConstBufferSequence>
        void send(const ConstBufferSequence& buffers);

        void setCredentials(const std::string& login,
                            const std::string& password);

        void setErrorCallback(const ErrorCallback& cb) { m_errorCallback = cb; }
        void setDataCallback(const DataCallback& cb) { m_dataCallback = cb; }
        void setEOFCallback(const EOFCallback& cb) { m_eofCallback = cb; }
        void setHeadersCallback(const HeadersCallback& cb) { m_headersCallback = cb; }

        void resetErrorCallback() { m_errorCallback = {}; }
        void resetDataCallback() { m_dataCallback = {}; }
        void resetEOFCallback() { m_eofCallback = {}; }
        void resetHeadersCallback() { m_headersCallback = {}; }

        const std::map<std::string, std::string>& headers() const { return m_headers; }

        bool isActive() const { return m_active; }

    protected:
        using tcp = boost::asio::ip::tcp;

        std::string m_server;
        uint16_t m_port;
        std::string m_uri;
        Authenticator m_auth;
        unsigned m_timeout;
        std::map<std::string, std::string> m_headers;
        tcp::socket m_socket;
        boost::asio::deadline_timer m_timeouter;
        boost::asio::streambuf m_request;

        virtual void prepareRequest() = 0;

    private:
        tcp::resolver m_resolver;
        boost::asio::streambuf m_response;
        ErrorCallback m_errorCallback;
        DataCallback m_dataCallback;
        EOFCallback m_eofCallback;
        HeadersCallback m_headersCallback;
        bool m_chunked;
        bool m_active;

        void handleResolve(const boost::system::error_code& error,
                           tcp::resolver::iterator it);
        void handleConnect(const boost::system::error_code& error,
                           tcp::resolver::iterator it);
        void handleWriteRequest(const boost::system::error_code& error);
        void handleWriteData(const boost::system::error_code& error);
        void handleReadStatus(const boost::system::error_code& error);
        void handleReadHeaders(const boost::system::error_code& error);
        void handleReadData(const boost::system::error_code& error);
        void handleReadChunkLength(const boost::system::error_code& error);
        void handleReadChunkData(const boost::system::error_code& error,
                                 size_t size);

        void shutdown();

        void restartTimer();
        void handleTimeout(const boost::system::error_code& ec);

        void reportError(const boost::system::error_code& ec);
        void reportError(int val);
};

template <typename ConstBufferSequence>
inline
void Connection::send(const ConstBufferSequence& buffers)
{
    if (m_timeout)
        m_timeouter.expires_from_now(boost::posix_time::seconds(m_timeout));

    async_write(
        m_socket,
        buffers,
        boost::asio::transfer_all(),
        std::bind(&Connection::handleWriteData, this, std::placeholders::_1)
    );
}

}

#endif
