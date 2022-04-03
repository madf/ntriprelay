#include "authenticator.h"
#include "base64.h"
#include "error.h"

using Caster::Authenticator;
using Caster::CasterError;

Authenticator::Authenticator() noexcept
    : m_authenticated(false)
{
}

Authenticator::Authenticator(const std::string& login,
                             const std::string& password) noexcept
    : m_login(login),
      m_password(password),
      m_authenticated(true)
{
}

std::string Authenticator::basic() const
{
    std::string credentials = m_login + ":" + m_password;
    return base64_encode(reinterpret_cast<const unsigned char*>(credentials.c_str()), credentials.length());
}

std::string Authenticator::digest(const std::string& /*method*/,
                                  const std::string& /*uri*/,
                                  const std::string& /*realm*/,
                                  uint8_t /*qop*/,
                                  const std::string& /*nonce*/) const
{
    throw CasterError("Digest authentication method is not implemented");
}

void Authenticator::setLogin(const std::string& login) noexcept
{
    m_login = login;
    m_authenticated = true;
}

void Authenticator::setPassword(const std::string& password) noexcept
{
    m_password = password;
    m_authenticated = true;
}
