#include <boost/bind.hpp>

#include "relay.h"

using Caster::Relay;

Relay::Relay(boost::asio::io_service& ioService,
             const std::string& srcServer, uint16_t srcPort,
             const std::string& srcMountpoint,
             const std::string& dstServer, uint16_t dstPort,
             const std::string& dstMountpoint)
    : m_client(ioService, srcServer, srcPort, srcMountpoint),
      m_server(ioService, dstServer, dstPort, dstMountpoint),
      m_errorCallback(),
      m_eofCallback()
{
}

Relay::~Relay()
{
}

void Relay::m_initCallbacks()
{
    m_client.setErrorCallback(
        boost::bind(
            &Relay::m_handleError,
            shared_from_this(),
            _1
        )
    );
    m_client.setDataCallback(
        boost::bind(
            &Relay::m_handleData,
            shared_from_this(),
            _1
        )
    );
    m_client.setEOFCallback(
        boost::bind(
            &Relay::m_handleEOF,
            shared_from_this()
        )
    );
    m_server.setErrorCallback(
        boost::bind(
            &Relay::m_handleError,
            shared_from_this(),
            _1
        )
    );
}

void Relay::m_clearCallbacks()
{
    m_client.resetErrorCallback();
    m_client.resetDataCallback();
    m_client.resetEOFCallback();
    m_server.resetErrorCallback();
}

void Relay::m_handleError(const boost::system::error_code& ec)
{
    if (!m_errorCallback.empty())
        m_errorCallback(ec);
    m_clearCallbacks();
    m_client.stop();
    m_server.stop();
}

void Relay::m_handleData(const boost::asio::const_buffers_1& buffers)
{
    if (m_server.isActive())
        m_server.send(buffers);
}

void Relay::m_handleEOF()
{
    if (!m_eofCallback.empty())
        m_eofCallback();
    m_clearCallbacks();
    m_client.stop();
    m_server.stop();
}
