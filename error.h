#ifndef __CASTER_ERROR_H__
#define __CASTER_ERROR_H__

#include <stdexcept>

namespace Caster {

class CasterError : public std::runtime_error {
    public:
        CasterError(const char * msg) : std::runtime_error(msg) {}
        CasterError(const std::string & msg) : std::runtime_error(msg) {}
};

}

#endif
