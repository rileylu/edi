#pragma once
#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include <memory>
#include <iostream>

class BaseSession : Noncopyable {
public:
    virtual ~BaseSession();
    std::iostream& io();

protected:
    BaseSession(st_utime_t timeout = 60 * UNIT);
    void set_istream(IStream& is);
    st_utime_t timeout_;
    void flush();

private:
    std::unique_ptr<STStreamBuf> buf_;
    std::unique_ptr<std::iostream> io_;
};
