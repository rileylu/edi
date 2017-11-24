#pragma once
#include <type_traits>

class IStream {
public:
    virtual ~IStream() = default;
    virtual int read(char* buf, std::size_t sz) = 0;
    virtual int write(const char* buf, std::size_t sz) = 0;

protected:
    IStream() = default;
};
