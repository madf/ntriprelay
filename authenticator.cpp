#include "base64.h"
#include "error.h"
#include "authenticator.h"

using Caster::Authenticator;
using Caster::CasterError;

Authenticator::Authenticator()
    : _login(),
      _password(),
      _authenticated(false)
{
}

Authenticator::Authenticator(const std::string & login,
                             const std::string & password)
    : _login(login),
      _password(password),
      _authenticated(true)
{
}

std::string Authenticator::basic() const
{
    std::string credentials = _login + ":" + _password;
    return base64_encode(reinterpret_cast<const unsigned char *>(credentials.c_str()), credentials.length());
}

std::string Authenticator::digest(const std::string & /*method*/,
                                  const std::string & /*uri*/,
                                  const std::string & /*realm*/,
                                  uint8_t /*qop*/,
                                  const std::string & /*nonce*/) const
{
    throw CasterError("Digest authentication method is not implemented");
}

void Authenticator::setLogin(const std::string & login)
{
    _login = login;
    _authenticated = true;
}

void Authenticator::setPassword(const std::string & password)
{
    _password = password;
    _authenticated = true;
}
