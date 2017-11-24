#pragma once
#include "noncopyable.hpp"
#include <exception>
#include <st.h>

class STMutex : Noncopyable {
public:
    STMutex();
    ~STMutex();
    void lock();
    void unlock();

private:
    st_mutex_t mutex_;
};
