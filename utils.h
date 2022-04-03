#ifndef __CASTER_UTILS_H__
#define __CASTER_UTILS_H__

#include <boost/asio/buffer.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include <algorithm>

namespace Caster {

inline
size_t parseChunkLength(const boost::asio::const_buffers_1& buffers,
                        size_t& length)
{
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;

    const char* const begin = boost::asio::buffer_cast<const char*>(*buffers.begin());
    const char* iter = begin;
    const char* const end(begin + boost::asio::buffer_size(*buffers.begin()));

    qi::parse(iter, end, (qi::hex[phoenix::ref(length) = qi::_1] >> *(qi::char_-'\r') > "\r\n"));

    return std::distance(begin, iter);
}

}

#endif
