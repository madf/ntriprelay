#pragma once

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

struct Error : std::runtime_error {
    explicit Error(const std::string& subsystem, const std::string& msg) noexcept : runtime_error("Caster::" + subsystem + ": " + msg) {}
};

class Category : public boost::system::error_category
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

        static Category& getInstance()
        {
            static Category category;
            return category;
        }
};

}
