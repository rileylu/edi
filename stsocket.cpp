//
//  stsocket.cpp
//  testst
//
//  Created by lmz on 19/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include <cstdio>
#include <exception>
#include "stsocket.hpp"

STSocket::~STSocket() {
    if(sock_)
        st_netfd_close(sock_);
}


STSocket::STSocket(st_utime_t timeout)
:timeout_(timeout)
{
    native_sock_=::socket(AF_INET, SOCK_STREAM, 0);
    if(native_sock_<0)
        throw;
    sock_=st_netfd_open_socket(native_sock_);
    if(sock_==nullptr)
        throw;
}

ssize_t STSocket::read(void *data, size_t len) {
    size_t nleft=len;
    ssize_t nread;
    char* ptr=reinterpret_cast<char*>(data);
    while(nleft>0)
    {
        if((nread=st_read(sock_,ptr, nleft, timeout_))<0)
        {
            if(errno==EINTR)
                nread=0;
            else
                //return -1;
                throw std::exception();
        }
        else if(nread==0)
            break;
        nleft-=nread;
        ptr+=nread;
    }
    return len-nleft;
}

ssize_t STSocket::write(const void *data, size_t len) {
    size_t nleft=len;
    ssize_t nwriten=0;
    const char* ptr=reinterpret_cast<const char*>(data);
    while(nleft>0)
    {
        nwriten=st_write(sock_, ptr, nleft, timeout_);
        if(nwriten<=0)
        {
            if(nwriten<0 && errno==EINTR)
                nwriten=0;
            else
//                return -1;
                throw std::exception();
        }
        nleft-=nwriten;
        ptr+=nwriten;
    }
    return len;
}

int STSocket::connect(struct sockaddr *addr, int addrlen) {
    int res=st_connect(sock_,addr,addrlen,timeout_);
    if(res<0)
        throw std::exception();
    return res;
}





