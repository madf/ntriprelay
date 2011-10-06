#include <boost/bind.hpp>

#include "relay.h"

using Caster::Relay;

Relay::Relay(boost::asio::io_service & ioService,
             const std::string & srcServer, uint16_t srcPort,
             const std::string & srcMountpoint,
             const std::string & dstServer, uint16_t dstPort,
             const std::string & dstMountpoint)
    : _clientPtr(new Client(ioService, srcServer, srcPort, srcMountpoint)),
      _serverPtr(new Server(ioService, dstServer, dstPort, dstMountpoint)),
      _errorCallback(),
      _eofCallback()
{
    _clientPtr->setErrorCallback(
        boost::bind(
            &Relay::_handleError,
            shared_from_this(),
            _1
        )
    );
    _clientPtr->setDataCallback(
        boost::bind(
            &Relay::_handleData,
            shared_from_this(),
            _1,
            _2
        )
    );
    _clientPtr->setEOFCallback(
        boost::bind(
            &Relay::_handleEOF,
            shared_from_this()
        )
    );
    _serverPtr->setErrorCallback(
        boost::bind(
            &Relay::_handleError,
            shared_from_this(),
            _1
        )
    );
}

void Relay::_handleError(const boost::system::error_code & ec)
{
    if (!_errorCallback.empty())
        _errorCallback(ec);
    if (_clientPtr->isActive())
        _clientPtr->stop();
    if (_serverPtr->isActive())
        _serverPtr->stop();
}

void Relay::_handleData(const boost::array<char, 1024> & data,
                        size_t amount)
{
    if (_serverPtr->isActive())
        _serverPtr->send(boost::asio::buffer(data, amount));
}

void Relay::_handleEOF()
{
    if (!_eofCallback.empty())
        _eofCallback();
    if (_clientPtr->isActive())
        _clientPtr->stop();
    if (_serverPtr->isActive())
        _serverPtr->stop();
}
