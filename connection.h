#ifndef __CASTER_CONNECTION_H__
#define __CASTER_CONNECTION_H__

#include "authenticator.h"
#include "callbacks.h"

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/bind.hpp>

#include <string>
#include <vector>
#include <map>

using boost::asio::ip::tcp;

namespace Caster {

class Connection : private boost::noncopyable {
    public:
        Connection(boost::asio::io_service& ioService,
                   const std::string& server, uint16_t port);

        Connection(boost::asio::io_service& ioService,
                   const std::string& server, uint16_t port,
                   const std::string& mountpoint);
        virtual ~Connection() {}

        void start();
        void start(unsigned timeout);
        void stop() { m_shutdown(); }

        template <typename ConstBufferSequence>
        void send(const ConstBufferSequence& buffers);

        void setCredentials(const std::string& login,
                            const std::string& password);

        void setErrorCallback(const ErrorCallback& cb) { m_errorCallback = cb; }
        void setDataCallback(const DataCallback& cb) { m_dataCallback = cb; }
        void setEOFCallback(const EOFCallback& cb) { m_eofCallback = cb; }
        void setHeadersCallback(const HeadersCallback& cb) { m_headersCallback = cb; }

        void resetErrorCallback() { m_errorCallback.clear(); }
        void resetDataCallback() { m_dataCallback.clear(); }
        void resetEOFCallback() { m_eofCallback.clear(); }
        void resetHeadersCallback() { m_headersCallback.clear(); }

        const std::map<std::string, std::string>& headers() const { return m_headers; }

        bool isActive() const { return m_active; }

    protected:
        std::string m_server;
        uint16_t m_port;
        std::string m_uri;
        Authenticator m_auth;
        unsigned m_timeout;
        std::map<std::string, std::string> m_headers;
        tcp::socket m_socket;
        boost::asio::deadline_timer m_timeouter;
        boost::asio::streambuf m_request;

        virtual void m_prepareRequest() = 0;

    private:
        tcp::resolver m_resolver;
        boost::asio::streambuf m_response;
        ErrorCallback m_errorCallback;
        DataCallback m_dataCallback;
        EOFCallback m_eofCallback;
        HeadersCallback m_headersCallback;
        bool m_chunked;
        bool m_active;

        void m_handleResolve(const boost::system::error_code& error,
                             tcp::resolver::iterator it);
        void m_handleConnect(const boost::system::error_code& error,
                             tcp::resolver::iterator it);
        void m_handleWriteRequest(const boost::system::error_code& error);
        void m_handleWriteData(const boost::system::error_code& error);
        void m_handleReadStatus(const boost::system::error_code& error);
        void m_handleReadHeaders(const boost::system::error_code& error);
        void m_handleReadData(const boost::system::error_code& error);
        void m_handleReadChunkLength(const boost::system::error_code& error);
        void m_handleReadChunkData(const boost::system::error_code& error,
                                   size_t size);
        void m_handleTimeout();
        void m_shutdown();
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
        boost::bind(
            &Connection::m_handleWriteData,
            this,
            boost::asio::placeholders::error
        )
    );
}

}

#endif
