#pragma once
#include "istream.h"
#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include "utilities.hpp"
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <st.h>

class FileStream : public std::iostream, public IStream, Noncopyable {
public:
    FileStream(const std::string& path, int flags, mode_t mode = 0755, st_utime_t timeout = 60 * edi::TIMEOUT_UNIT);
    ~FileStream();

protected:
    int read(char* buf, std::size_t sz) override;
    int write(const char* buf, std::size_t sz) override;

private:
    st_netfd_t fd_;
    st_utime_t timeout_;
    std::unique_ptr<STStreamBuf> buf_;
};
