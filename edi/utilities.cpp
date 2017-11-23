#include "utilities.hpp"

addrinfo edi::getaddrinfo(const std::string& host, const std::string& servname)
{
    addrinfo hint, *res0 = nullptr;
    int err;
    bzero(&hint, sizeof(hint));
    hint.ai_family = PF_INET;
    hint.ai_socktype = SOCK_STREAM;
    err = ::getaddrinfo(host.c_str(), servname.c_str(), &hint, &res0);
    if (err) {
        if (res0)
            freeaddrinfo(res0);
        throw std::exception();
    }
    addrinfo res;
    bzero(&res, sizeof(addrinfo));
    ::memcpy(&res, res0, sizeof(addrinfo));
    if (res0)
        ::freeaddrinfo(res0);
    return res;
}
