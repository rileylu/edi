//
// Created by lmz on 21/11/2017.
//

#include "session.h"
#include "utilities.hpp"

Session::Session(const std::string &host, const std::string &port, st_utime_t timeout)
        : host_(host), port_(port), stSocket_(), inbuf_(stSocket_), is_(&inbuf_), outbuf_(stSocket_), os_(&outbuf_),
          timeout_(timeout) {
}

void Session::open() {
    addrinfo addr = edi::getaddrinfo(host_, port_);
    stSocket_.set_timeout(timeout_);
    stSocket_.connect(addr.ai_addr, addr.ai_addrlen);
}

std::istream &Session::read_data() {
    return is_;
}

std::ostream &Session::write_data() {
    return os_;
}

