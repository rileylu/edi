//
//  ststreambuf.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "ststreambuf.hpp"

std::streamsize STStreamBuf::xsputn(const char_type *s, std::streamsize n) {
    size_t nleft = n;
    const char *sptr = s;
    int bytes_moved = 0;
    while (nleft > 0) {
        bytes_moved = std::min((size_t) (epptr() - pptr()), nleft);
        memmove(pptr(), sptr, bytes_moved);
        pbump(bytes_moved);
        if (pptr() == epptr()) {
            if (flushbuffer() == EOF)
                return EOF;
            setp(buf_.data(), buf_.data() + BUFSIZE - 1);
        }
        sptr += bytes_moved;
        nleft -= bytes_moved;
    }
    return n;
}

STStreamBuf::int_type STStreamBuf::overflow(int_type c) {
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
        if (flushbuffer() == EOF)
            return EOF;
    }
    return c;
}

int STStreamBuf::flushbuffer() {
    int num = pptr() - pbase();
    if (istream_.write(buf_.data(), num) != num)
        return EOF;
    pbump(-num);
    return num;
}

STStreamBuf::~STStreamBuf() {
    sync();
}

int STStreamBuf::sync() {
    if (flushbuffer() == EOF)
        return -1;
    return 1;
}

int STStreamBuf::underflow() {
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }
    int num;
    num = istream_.read(buf_.data(), BUFSIZE);
    if (num <= 0) {
        return EOF;
    }
    setg(buf_.data() ,
         buf_.data() ,
         buf_.data()+num);
    return traits_type::to_int_type(*gptr());
}
