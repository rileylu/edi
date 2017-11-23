//
//  filestream.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "filestream.hpp"
#include <exception>
ssize_t FileStream::write(const void* buf, size_t len, st_utime_t timeout)
{
    ssize_t n = 0;
    do {
        n = st_write(des_, buf, len, timeout);
    } while (n < 0 && errno == EINTR);
    if (n < len)
        throw std::exception();
    return len;
}

ssize_t FileStream::read(void* buf, size_t len, st_utime_t timeout)
{
    ssize_t n = 0;
    do {
        n = st_read(des_, buf, len, timeout);
    } while (n < 0 && errno == EINTR);
    if (n < 0)
        throw std::exception();
    return n;
}

FileStream::~FileStream()
{
    if (des_)
        st_netfd_close(des_);
}

FileStream::FileStream()
    : des_(0)
{
}

st_netfd_t FileStream::get_des() const
{
    return des_;
}

void FileStream::set_des(st_netfd_t des)
{
    des_ = des;
}
