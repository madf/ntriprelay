#ifndef __CASTER_AUTHENTICATOR_H__
#define __CASTER_AUTHENTICATOR_H__

#include <string>
#include <cstdint>

namespace Caster {

class Authenticator {
    public:
        Authenticator();
        Authenticator(const std::string& login,
                      const std::string& password);

        void setLogin(const std::string& login);
        void setPassword(const std::string& password);

        std::string basic() const;
        std::string digest(const std::string& method,
                           const std::string& uri,
                           const std::string& realm,
                           uint8_t qop,
                           const std::string& nonce) const;

        bool authenticated() const { return m_authenticated; }

    private:
        std::string m_login;
        std::string m_password;
        bool m_authenticated;
};

}

#endif
