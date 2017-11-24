#include "filestream.hpp"
FileStream::FileStream(const std::string& path, int flags, mode_t mode, st_utime_t timeout)
    : std::iostream(nullptr)
    , fd_(0)
    , timeout_(timeout)
    , buf_(nullptr)
{
    fd_ = st_open(path.c_str(), flags, mode);
    if (fd_ == nullptr)
        throw std::exception();
    buf_.reset(new STStreamBuf(this));
    rdbuf(buf_.get());
}

FileStream::~FileStream()
{
    if (buf_)
        buf_.reset(nullptr);
    if (fd_)
        st_netfd_close(fd_);
}

int FileStream::read(char* buf, std::size_t sz)
{
    ssize_t n = 0;
    do {
        n = st_read(fd_, buf, sz, timeout_);
    } while (n < 0 && errno == EINTR);
    if (n < 0)
        throw std::exception();
    return n;
}

int FileStream::write(const char* buf, std::size_t sz)
{
    ssize_t n = 0;
    do {
        n = st_write(fd_, buf, sz, timeout_);
    } while (n < 0 && errno == EINTR);
    if (n < 0)
        throw std::exception();
    return n;
}
