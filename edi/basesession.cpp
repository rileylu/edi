#include "basesession.hpp"

BaseSession::BaseSession(st_utime_t timeout)
    : buf_(nullptr)
    , io_(nullptr)
    , timeout_(timeout)
{
}

BaseSession::~BaseSession()
{
    if (buf_)
        buf_.reset(nullptr);
    if (io_)
        io_.reset(nullptr);
}

void BaseSession::flush()
{
    io_->sync();
}

std::iostream& BaseSession::io()
{
    return *io_;
}

void BaseSession::set_istream(IStream& is)
{
    buf_.reset(new STStreamBuf(is));
    io_.reset(new std::iostream(buf_.get()));
}
