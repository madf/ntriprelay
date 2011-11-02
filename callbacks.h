#ifndef __CASTER_CALLBACKS_H__
#define __CASTER_CALLBACKS_H__

#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <boost/array.hpp>

namespace Caster {

typedef boost::function<void (const boost::system::error_code & code)> ErrorCallback;
typedef boost::function<void (const boost::array<char, 2048> & data,
                              size_t amount)> DataCallback;
typedef boost::function<void ()> EOFCallback;
typedef boost::function<void ()> HeadersCallback;

}

#endif
