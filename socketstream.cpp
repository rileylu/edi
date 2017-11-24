#include "socketstream.hpp"
#include "utilities.hpp"

SocketStream::SocketStream(const std::string& host, const std::string port, st_utime_t timeout)
    : std::iostream(nullptr)
    , fd_(0)
    , timeout_(timeout)
    , buf_(nullptr)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::exception();
    bool f = true;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &f, sizeof(f));
    linger l = { 0, 0 };
    ::setsockopt(fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
    fd_ = st_netfd_open_socket(fd);
    if (fd_ == 0)
        throw std::exception();
    addrinfo addr = edi::getaddrinfo(host, port);
    if (st_connect(fd_, addr.ai_addr, addr.ai_addrlen, timeout) < 0)
        throw std::exception();
    buf_.reset(new STStreamBuf(this));
    rdbuf(buf_.get());
}

SocketStream::~SocketStream()
{
    if (buf_)
        buf_.reset(nullptr);
    if (fd_) {
        int fd = st_netfd_fileno(fd_);
        ::shutdown(fd, SHUT_RDWR);
        st_netfd_close(fd_);
    }
}

int SocketStream::read(char* buf, std::size_t sz)
{
    ssize_t nread = st_read(fd_, buf, sz, timeout_);
    if (nread < 0)
        throw std::exception();
    return nread;
}

int SocketStream::write(const char* buf, std::size_t sz)
{
    size_t nleft = sz;
    ssize_t nwriten = 0;
    const char* ptr = reinterpret_cast<const char*>(buf);
    while (nleft > 0) {
        nwriten = st_write(fd_, ptr, nleft, timeout_);
        if (nwriten <= 0) {
            if (nwriten < 0 && errno == EINTR)
                nwriten = 0;
            else
                throw std::exception();
        }
        nleft -= nwriten;
        ptr += nwriten;
    }
    return sz;
}
