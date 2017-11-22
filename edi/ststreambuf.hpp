//
//  ststreambuf.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef ststreambuf_hpp
#define ststreambuf_hpp
#include "istream.hpp"
#include "noncopyable.hpp"
#include <streambuf>
#include <array>

class STStreamBuf:Noncopyable,public std::streambuf
{
public:
    const static std::size_t BUFSIZE = 1024;
    
    STStreamBuf(IStream &is)
    : istream_(is),buf_{0}{
        setp(buf_.data(), buf_.data() + (BUFSIZE - 1));
        setg(buf_.data(), buf_.data(), buf_.data());
    }
    
    virtual ~STStreamBuf();
    
protected:
    int_type overflow(int_type __c = traits_type::eof()) override;
    
    std::streamsize xsputn(const char_type *__s, std::streamsize __n) override;
    
    int_type underflow() override;
    
    int flushbuffer();
    
    int sync() override;
    
private:
    IStream &istream_;
    std::array<char,BUFSIZE> buf_;
};

#endif /* ststreambuf_hpp */
