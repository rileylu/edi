#pragma once
#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include "utilities.hpp"
#include <iostream>
#include <memory>
#include <st.h>
#include <string>

class SocketStream : public std::iostream, Noncopyable {
public:
    SocketStream(const std::string& host, const std::string port, st_utime_t timeout = 60 * edi::TIMEOUT_UNIT);
    ~SocketStream();

private:
    class SocketStreamBuf : public STStreamBuf {
    public:
        SocketStreamBuf(st_netfd_t fd, st_utime_t timeout);
        ssize_t read(char_type* buf, std::streamsize sz) override;
        ssize_t write(const char_type* buf, std::streamsize sz) override;

    private:
        st_netfd_t fd_;
        st_utime_t timeout_;
    };
    st_netfd_t fd_;
    st_utime_t timeout_;
    std::unique_ptr<SocketStreamBuf> buf_;
};
