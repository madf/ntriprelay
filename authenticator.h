#ifndef __CASTER_AUTHENTICATOR_H__
#define __CASTER_AUTHENTICATOR_H__

#include <string>
#include <cstdint>

namespace Caster {

class Authenticator {
    public:
        Authenticator() noexcept;
        Authenticator(const std::string& login,
                      const std::string& password) noexcept;

        void setLogin(const std::string& login) noexcept;
        void setPassword(const std::string& password) noexcept;

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

#endif
