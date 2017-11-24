#include "filestream.hpp"
FileStream::FileStream(const std::string& path, int flags, mode_t mode, st_utime_t timeout)
    : std::iostream()
    , fd_(0)
    , timeout_(timeout)
    , buf_(nullptr)
{
    fd_ = st_open(path.c_str(), flags, mode);
    if (fd_ == nullptr)
        throw std::exception();
    buf_.reset(new FileStreamBuf(fd_, timeout));
    rdbuf(buf_.get());
}

FileStream::~FileStream()
{
    if (buf_)
        buf_.reset(nullptr);
    if (fd_)
        st_netfd_close(fd_);
}

FileStream::FileStreamBuf::FileStreamBuf(st_netfd_t fd, st_utime_t timeout)
    : fd_(fd)
    , timeout_(timeout)
{
}

ssize_t FileStream::FileStreamBuf::read(STStreamBuf::char_type* buf, std::streamsize sz)
{
    ssize_t n = 0;
    do {
        n = st_read(fd_, buf, sz, timeout_);
    } while (n < 0 && errno == EINTR);
    if (n < 0)
        throw std::exception();
    return n;
}

ssize_t FileStream::FileStreamBuf::write(const STStreamBuf::char_type* buf, std::streamsize sz)
{
    ssize_t n = 0;
    do {
        n = st_write(fd_, buf, sz, timeout_);
    } while (n < 0 && errno == EINTR);
    if (n < 0)
        throw std::exception();
    return n;
}
