#ifndef __CASTER_ERROR_H__
#define __CASTER_ERROR_H__

#include <boost/system/error_code.hpp>

#include <stdexcept>

namespace Caster {

enum {
    success,
    resolveError,
    invalidStatus,
    connectionTimeout,
    invalidChunkLength
};

struct CasterError : std::runtime_error {
    explicit CasterError(const std::string& msg) noexcept : std::runtime_error(msg) {}
};

class CasterCategory : public boost::system::error_category
{
    public:
        const char* name() const BOOST_SYSTEM_NOEXCEPT { return "Caster"; }
        std::string message(int ev) const
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

        static CasterCategory& getInstance()
        {
            static CasterCategory category;
            return category;
        }
};

}

#endif
