#pragma once
#include "istream.hpp"
#include "noncopyable.hpp"
#include <array>
#include <streambuf>

class STStreamBuf : Noncopyable, public std::streambuf {
public:
    const static std::size_t BUFSIZE = 1024;
    STStreamBuf(IStream& is)
        : istream_(is)
    {
        reset();
    }
    virtual ~STStreamBuf();
protected:
    void reset()
    {
        setp(outbuf_.data(), outbuf_.data() + BUFSIZE);
        setg(inbuf_.data()+4, inbuf_.data()+4, inbuf_.data()+4);
    }
    int_type overflow(int_type __c = traits_type::eof()) override;

    int_type underflow() override;

    int flushbuffer();

    int sync() override;

private:
    IStream& istream_;
    std::array<char, BUFSIZE> inbuf_;
    std::array<char, BUFSIZE> outbuf_;
};
