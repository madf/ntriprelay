#ifndef __CASTER_UTILS_H__
#define __CASTER_UTILS_H__

#include <algorithm>

#include <boost/asio/buffer.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace Caster {

inline
size_t parseChunkLength(const boost::asio::const_buffers_1 & buffers,
                        size_t & length)
{
    assert(buffers.begin() != buffers.end());

    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;

    boost::asio::const_buffers_1::value_type::const_iterator begin(buffers.begin()->begin());
    const boost::asio::const_buffers_1::value_type::const_iterator end(buffers.begin()->end());

    qi::parse(begin, end, (qi::hex[phoenix::ref(length) = qi::_1] >> +(qi::char_-'\r') > "\r\n"));
    return std::distance(begin, end);
}

}

#endif
