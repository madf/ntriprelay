#pragma once

#include "error.h"

#include <string>
#include <cstdint>

namespace Caster
{

class Authenticator
{
    public:
        struct Error : Caster::Error
        {
            explicit Error(const std::string& message) noexcept : Caster::Error("Authenticator", message) {}
        };

        Authenticator() noexcept;
        Authenticator(const std::string& login,
                      const std::string& password) noexcept;

        Authenticator(const Authenticator&) = default;
        Authenticator& operator=(const Authenticator&) = default;
        Authenticator(Authenticator&&) = default;
        Authenticator& operator=(Authenticator&&) = default;

        std::string basic() const;
        std::string digest(const std::string& method,
                           const std::string& uri,
                           const std::string& realm,
                           uint8_t qop,
                           const std::string& nonce) const;

        bool authenticated() const noexcept { return m_authenticated; }

    private:
        std::string m_login;
        std::string m_password;
        bool m_authenticated;
};

}
