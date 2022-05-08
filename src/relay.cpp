#include "relay.h"

#include <functional> // std::bind

using Caster::Relay;

namespace pls = std::placeholders;
namespace bs = boost::system;
namespace ba = boost::asio;

Relay::Relay(ba::io_service& ioService,
             const std::string& srcServer, uint16_t srcPort,
             const std::string& srcMountpoint,
             const std::string& dstServer, uint16_t dstPort,
             const std::string& dstMountpoint)
    : m_client(ioService, srcServer, srcPort, srcMountpoint),
      m_server(ioService, dstServer, dstPort, dstMountpoint)
{
}

void Relay::initCallbacks()
{
    m_client.setErrorCallback(
        std::bind(
            &Relay::handleClientError,
            shared_from_this(),
            pls::_1
        )
    );
    m_client.setDataCallback(
        std::bind(
            &Relay::handleData,
            shared_from_this(),
            pls::_1
        )
    );
    m_client.setEOFCallback(
        std::bind(
            &Relay::handleEOF,
            shared_from_this()
        )
    );
    m_server.setErrorCallback(
        std::bind(
            &Relay::handleServerError,
            shared_from_this(),
            pls::_1
        )
    );
}

void Relay::clearCallbacks()
{
    m_client.resetErrorCallback();
    m_client.resetDataCallback();
    m_client.resetEOFCallback();
    m_server.resetErrorCallback();
}

void Relay::handleClientError(const bs::error_code& ec)
{
    handleError("Source connection error: " + ec.message());
}

void Relay::handleServerError(const bs::error_code& ec)
{
    handleError("Destination connection error: " + ec.message());
}

void Relay::handleError(const std::string& message)
{
    if (m_errorCallback)
        m_errorCallback(message);
    clearCallbacks();
    m_client.stop();
    m_server.stop();
}

void Relay::handleData(const ba::const_buffers_1& buffers)
{
    if (m_server.isActive())
        m_server.send(buffers);
}

void Relay::handleEOF()
{
    if (m_eofCallback)
        m_eofCallback();
    clearCallbacks();
    m_client.stop();
    m_server.stop();
}
