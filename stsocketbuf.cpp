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
    return sock_.write(s, n);
}
STSocketBuf::int_type STSocketBuf::overflow(int_type c) {
    if(c!=EOF)
    {
        char z=c;
        if(sock_.write(&z, 1)!=1)
            return EOF;
    }
    return c;
}
