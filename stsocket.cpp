//
//  stsocket.cpp
//  testst
//
//  Created by lmz on 19/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include <cstdio>
#include <exception>
#include <functional>
#include "stsocket.hpp"

STSocket::~STSocket() {
    if (sock_)
        st_netfd_close(sock_);
}


STSocket::STSocket() {
    native_sock_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (native_sock_ < 0)
        throw;
    sock_ = st_netfd_open_socket(native_sock_);
    if (sock_ == nullptr)
        throw;
    st_netfd_setspecific(sock_, &native_sock_, &STSocket::sock_destructor);
}

void STSocket::sock_destructor(void *args) {
    int *fd = reinterpret_cast<int *>(args);
    if (::shutdown(*fd, SHUT_RDWR) < 0)
        throw std::exception();
}

ssize_t STSocket::read(void *data, size_t len) {
    ssize_t nread = st_read(sock_, data, len, timeout_);
    if (nread < 0)
        throw std::exception();
    return nread;
}

ssize_t STSocket::write(const void *data, size_t len) {
    size_t nleft = len;
    ssize_t nwriten = 0;
    const char *ptr = reinterpret_cast<const char *>(data);
    while (nleft > 0) {
        nwriten = st_write(sock_, ptr, nleft, timeout_);
        if (nwriten <= 0) {
            if (nwriten < 0 && errno == EINTR)
                nwriten = 0;
            else
                throw std::exception();
        }
        nleft -= nwriten;
        ptr += nwriten;
    }
    return len;
}

int STSocket::connect(struct sockaddr *addr, int addrlen) {
    int res = st_connect(sock_, addr, addrlen, timeout_);
    if (res < 0)
        throw std::exception();
    return res;
}





