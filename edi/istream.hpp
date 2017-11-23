#pragma once
#include <st.h>
#include <sys/types.h>
#define UNIT 1000000

class IStream {
public:
    virtual ~IStream() = default;
    virtual ssize_t read(void* buf, size_t len, st_utime_t timeout = 60 * UNIT) = 0;
    virtual ssize_t write(const void* buf, size_t len, st_utime_t timeout = 60 * UNIT) = 0;
    virtual st_netfd_t get_des() const = 0;

protected:
    IStream() = default;
};
