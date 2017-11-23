#include "basesession.hpp"

BaseSession::BaseSession(st_utime_t timeout)
    : inbuf_(nullptr)
    , outbuf_(nullptr)
    , io_(nullptr)
    , timeout_(timeout)
{
}

BaseSession::~BaseSession()
{
    if (inbuf_)
        inbuf_.reset(nullptr);
    if (outbuf_)
        outbuf_.reset(nullptr);
    if (io_)
        io_.reset(nullptr);
}

void BaseSession::flush()
{
    io_->sync();
}

BufferedIOStream& BaseSession::io()
{
    return *io_;
}

void BaseSession::set_istream(IStream& is)
{
    inbuf_.reset(new STStreamBuf(is));
    outbuf_.reset(new STStreamBuf(is));
    io_.reset(new BufferedIOStream(*inbuf_, *outbuf_));
}
