#include <boost/bind.hpp>

#include "relay.h"

using Caster::Relay;

Relay::Relay(boost::asio::io_service & ioService,
             const std::string & srcServer, uint16_t srcPort,
             const std::string & srcMountpoint,
             const std::string & dstServer, uint16_t dstPort,
             const std::string & dstMountpoint)
    : _client(ioService, srcServer, srcPort, srcMountpoint),
      _server(ioService, dstServer, dstPort, dstMountpoint),
      _errorCallback(),
      _eofCallback()
{
}

Relay::~Relay()
{
}

void Relay::_initCallbacks()
{
    _client.setErrorCallback(
        boost::bind(
            &Relay::_handleError,
            shared_from_this(),
            _1
        )
    );
    _client.setDataCallback(
        boost::bind(
            &Relay::_handleData,
            shared_from_this(),
            _1,
            _2
        )
    );
    _client.setEOFCallback(
        boost::bind(
            &Relay::_handleEOF,
            shared_from_this()
        )
    );
    _server.setErrorCallback(
        boost::bind(
            &Relay::_handleError,
            shared_from_this(),
            _1
        )
    );
}

void Relay::_clearCallbacks()
{
    _client.resetErrorCallback();
    _client.resetDataCallback();
    _client.resetEOFCallback();
    _server.resetErrorCallback();
}

void Relay::_handleError(const boost::system::error_code & ec)
{
    if (!_errorCallback.empty())
        _errorCallback(ec);
    _clearCallbacks();
    _client.stop();
    _server.stop();
}

void Relay::_handleData(const boost::asio::const_buffers_1 & buffers)
{
    if (_server.isActive())
        _server.send(buffers);
}

void Relay::_handleEOF()
{
    if (!_eofCallback.empty())
        _eofCallback();
    _clearCallbacks();
    _client.stop();
    _server.stop();
}
