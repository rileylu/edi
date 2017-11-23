//
//  socketstream.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "socketstream.hpp"
#include <exception>

ssize_t SocketStream::write(const void *buf, size_t len,st_utime_t timeout) {
    size_t nleft = len;
    ssize_t nwriten = 0;
    const char *ptr = reinterpret_cast<const char *>(buf);
    while (nleft > 0) {
        nwriten = st_write(des_, ptr, nleft, timeout);
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

ssize_t SocketStream::read(void *buf, size_t len,st_utime_t timeout) {
    ssize_t nread = st_read(des_, buf, len, timeout);
    if (nread < 0)
        throw std::exception();
    return nread;
}

void SocketStream::destructor(void *args) {
    int *fd=reinterpret_cast<int*>(args);
    ::shutdown(*fd, SHUT_RDWR);
}

SocketStream::~SocketStream(){
    if(des_)
        st_netfd_close(des_);
}


SocketStream::SocketStream() {
    int s=::socket(AF_INET,SOCK_STREAM,0);
    if(s<0)
        throw std::exception();
    des_=st_netfd_open_socket(s);
    if(des_==0)
        throw std::exception();
    st_netfd_setspecific(des_, &s, &SocketStream::destructor);
}


