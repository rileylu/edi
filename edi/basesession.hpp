#pragma once
#include "bufferediostream.h"
#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include <memory>

class BaseSession : Noncopyable {
public:
    virtual ~BaseSession();
    BufferedIOStream& io();

protected:
    BaseSession(st_utime_t timeout = 60 * UNIT);
    void set_istream(IStream& is);
    st_utime_t timeout_;
    void flush();

private:
    std::unique_ptr<STStreamBuf> inbuf_;
    std::unique_ptr<STStreamBuf> outbuf_;
    std::unique_ptr<BufferedIOStream> io_;
};
