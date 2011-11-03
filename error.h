#ifndef __CASTER_ERROR_H__
#define __CASTER_ERROR_H__

#include <stdexcept>

#include <boost/system/error_code.hpp>

namespace Caster {

enum {
    success,
    resolveError,
    invalidStatus,
    connectionTimeout,
    invalidChunkLength
};

class CasterError : public std::runtime_error {
    public:
        CasterError(const char * msg) : std::runtime_error(msg) {}
        CasterError(const std::string & msg) : std::runtime_error(msg) {}
};

class CasterCategory : public boost::system::error_category
{
    public:
        const char * name() const { return "Caster"; }
        std::string  message(int ev) const
        {
            switch (ev) {
                case success:
                    return "Success";
                case resolveError:
                    return "Failed to resolve supplied address";
                case invalidStatus:
                    return "Invalid status string";
                case connectionTimeout:
                    return "Connection timeout";
                case invalidChunkLength:
                    return "Invalid chunk length";
                default:
                    return "Unknown error";
            };
        }

        static CasterCategory & getInstance()
        {
            static CasterCategory category;
            return category;
        }
};

}

#endif
