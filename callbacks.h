#ifndef __CASTER_CALLBACKS_H__
#define __CASTER_CALLBACKS_H__

#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/buffer.hpp>

namespace Caster {

using ErrorCallback = boost::function<void (const boost::system::error_code& code)>;
using DataCallback = boost::function<void (const boost::asio::const_buffers_1& buffers)>;
using EOFCallback = boost::function<void ()>;
using HeadersCallback = boost::function<void ()>;

}

#endif
