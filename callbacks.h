#ifndef __CASTER_CALLBACKS_H__
#define __CASTER_CALLBACKS_H__

#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/buffer.hpp>

namespace Caster {

typedef boost::function<void (const boost::system::error_code& code)> ErrorCallback;
typedef boost::function<void (const boost::asio::const_buffers_1& buffers)> DataCallback;
typedef boost::function<void ()> EOFCallback;
typedef boost::function<void ()> HeadersCallback;

}

#endif
