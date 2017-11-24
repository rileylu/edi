#pragma once
#include "noncopyable.hpp"
#include <array>
#include <streambuf>

class STStreamBuf : Noncopyable, public std::streambuf {
public:
    const static std::size_t BUFSIZE = 1024;
    STStreamBuf()
    {
        reset();
    }
    virtual ~STStreamBuf();

protected:
    virtual ssize_t read(char_type* buf, std::streamsize sz)
    {
        return 0;
    }
    virtual ssize_t write(const char_type* buf, std::streamsize sz)
    {
        return 0;
    }

    void reset()
    {
        setp(outbuf_.data(), outbuf_.data() + BUFSIZE - 1);
        setg(inbuf_.data() + 4, inbuf_.data() + 4, inbuf_.data() + 4);
    }
    int_type overflow(int_type __c = traits_type::eof()) override;

    int_type underflow() override;

    int flushbuffer();

    int sync() override;

private:
    std::array<char, BUFSIZE> inbuf_;
    std::array<char, BUFSIZE> outbuf_;
};
