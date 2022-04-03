#ifndef __CASTER_CALLBACKS_H__
#define __CASTER_CALLBACKS_H__

#include <boost/system/error_code.hpp>
#include <boost/asio/buffer.hpp>

#include <functional>

namespace Caster {

using ErrorCallback = std::function<void (const boost::system::error_code&)>;
using DataCallback = std::function<void (const boost::asio::const_buffers_1&)>;
using EOFCallback = std::function<void ()>;
using HeadersCallback = std::function<void ()>;

}

#endif
