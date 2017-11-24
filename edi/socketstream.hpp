#pragma once
#include "istream.h"
#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include "utilities.hpp"
#include <iostream>
#include <memory>
#include <st.h>
#include <string>

class SocketStream : public std::iostream, Noncopyable, public IStream {
public:
    SocketStream(const std::string& host, const std::string port, st_utime_t timeout = 60 * edi::TIMEOUT_UNIT);
    ~SocketStream();

protected:
    int read(char* buf, std::size_t sz) override;
    int write(const char* buf, std::size_t sz) override;

private:
    st_netfd_t fd_;
    st_utime_t timeout_;
    std::unique_ptr<STStreamBuf> buf_;
};
