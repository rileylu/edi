//
//  stsocketbuf.cpp
//  testst
//
//  Created by lmz on 20/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "stsocketbuf.hpp"
#include "stsocket.hpp"

std::streamsize STSocketBuf::xsputn(const char_type *s, std::streamsize n) {
    size_t nleft = n;
    const char *sptr = s;
    int bytes_moved = 0;
    while (nleft > 0) {
        bytes_moved = std::min((size_t) (epptr() - pptr()), nleft);
        ::memmove(pptr(), sptr, bytes_moved);
        pbump(bytes_moved);
        if (pptr() == epptr()) {
            if (flushbuffer() == EOF)
                return EOF;
            setp(buf_.data()+putback_sz, buf_.data() + BUFSIZE - 1);
        }
        sptr += bytes_moved;
        nleft -= bytes_moved;
    }
    return n;
}

STSocketBuf::int_type STSocketBuf::overflow(int_type c) {
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
        if (flushbuffer() == EOF)
            return EOF;
    }
    return c;
}

int STSocketBuf::flushbuffer() {
    int num = pptr() - pbase();
    if (sock_.write(buf_.data()+putback_sz, num) != num)
        return EOF;
    pbump(-num);
    return num;
}

STSocketBuf::~STSocketBuf() {
    sync();
}

int STSocketBuf::sync() {
    if (flushbuffer() == EOF)
        return -1;
    return 1;
}

int STSocketBuf::underflow() {
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }
    int numPutback;
    numPutback = gptr() - eback();
    if (numPutback > 4) {
        numPutback = 4;
    }
    std::memmove(buf_.data() + (4 - numPutback), gptr() - numPutback,
                 numPutback);
    int num;
//    num = read (0, buf_.data()+4, BUFSIZE-4);
    num = sock_.read(buf_.data() + 4, BUFSIZE - 4);
    if (num <= 0) {
        return EOF;
    }
    setg(buf_.data() + (4 - numPutback),
         buf_.data() + 4,
         buf_.data() + 4 + num);
    return traits_type::to_int_type(*gptr());
}
