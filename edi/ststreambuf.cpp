//
//  ststreambuf.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "ststreambuf.hpp"

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
    int num = int(pptr() - pbase());
    if (istream_.write(outbuf_.data(), num) != num)
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
    int putback=int(gptr()-eback());
    if(putback>4)
        putback=4;
    memmove(inbuf_.data()+(4-putback), gptr()-putback,putback);
    int num;
    num = int(istream_.read(inbuf_.data()+4, BUFSIZE-4));
    if (num <= 0) {
        return EOF;
    }
    setg(inbuf_.data()+(4-putback) ,
         inbuf_.data()+4 ,
         inbuf_.data()+4+num);
    return traits_type::to_int_type(*gptr());
}

