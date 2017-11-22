//
//  utilities.hpp
//  testst
//
//  Created by lmz on 21/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef utilities_hpp
#define utilities_hpp

#include <sys/types.h>
#include <netdb.h>
#include <string>

namespace edi {
    addrinfo getaddrinfo(const std::string &host, const std::string &servname) {
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
        addrinfo res{0};
        ::memcpy(&res, res0, sizeof(addrinfo));
        if (res0)
            ::freeaddrinfo(res0);
        return res;
    }
}

#endif /* utilities_hpp */
