#pragma once
#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include "utilities.hpp"
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <st.h>

class FileStream : public std::iostream, Noncopyable {
public:
    FileStream(const std::string& path, int flags, mode_t mode = 0755, st_utime_t timeout = 60 * edi::TIMEOUT_UNIT);
    ~FileStream();

private:
    class FileStreamBuf : public STStreamBuf {
    public:
        FileStreamBuf(st_netfd_t fd, st_utime_t timeout);
        ssize_t read(char_type* buf, std::streamsize sz) override;
        ssize_t write(const char_type* buf, std::streamsize sz) override;

    private:
        st_netfd_t fd_;
        st_utime_t timeout_;
    };
    st_netfd_t fd_;
    st_utime_t timeout_;
    std::unique_ptr<FileStreamBuf> buf_;
};
