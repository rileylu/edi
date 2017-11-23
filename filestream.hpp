#pragma once
#include "istream.hpp"
#include "noncopyable.hpp"
#include <fcntl.h>
#include <st.h>

class FileStream : public IStream, Noncopyable {
public:
    FileStream();
    virtual ~FileStream();
    ssize_t read(void* buf, size_t len, st_utime_t timeout = 60 * UNIT) override;
    ssize_t write(const void* buf, size_t len, st_utime_t timeout = 60 * UNIT) override;
    st_netfd_t get_des() const override;
    void set_des(st_netfd_t des);

private:
    st_netfd_t des_;
};
