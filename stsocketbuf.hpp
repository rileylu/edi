//
//  stsocketbuf.hpp
//  testst
//
//  Created by lmz on 20/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef stsocketbuf_hpp
#define stsocketbuf_hpp

#include <streambuf>
#include <ostream>
#include <vector>

class STSocket;

class STSocketBuf : public std::streambuf {
public:
    const static std::size_t BUFSIZE = 512;

    STSocketBuf(STSocket &sock)
            : sock_(sock), buf_(BUFSIZE) {
        setp(buf_.data(), buf_.data() + (BUFSIZE - 1));
        setg(buf_.data()+4,buf_.data()+4,buf_.data()+4);
    }

    virtual ~STSocketBuf();

protected:
    int_type overflow(int_type __c = traits_type::eof()) override;

    std::streamsize xsputn(const char_type *__s, std::streamsize __n) override;

    int_type underflow() override ;

    int flushbuffer();

    int sync() override;

private:
    STSocket &sock_;
    std::vector<char> buf_;
};

#endif /* stsocketbuf_hpp */
