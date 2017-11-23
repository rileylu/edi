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
        setp(buf_.data(), buf_.data() + (BUFSIZE - 1));
        setg(buf_.data(), buf_.data(), buf_.data());
    }

    virtual ~STStreamBuf();

protected:
    int_type overflow(int_type __c = traits_type::eof()) override;

    int_type underflow() override;

    int flushbuffer();

    int sync() override;

private:
    IStream& istream_;
    std::array<char, BUFSIZE> buf_;
};
