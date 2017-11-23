#include "stmutex.h"

STMutex::STMutex()
    : mutex_(nullptr)
{
    mutex_ = st_mutex_new();
    if (mutex_ == nullptr)
        throw std::exception();
}

STMutex::~STMutex()
{
    st_mutex_destroy(mutex_);
}

void STMutex::lock()
{
    int res = 0;
    do {
        res = st_mutex_lock(mutex_);
    } while (res < 0 && errno == EINTR);
    if (res < 0)
        throw std::exception();
}

void STMutex::unlock()
{
    int res = st_mutex_unlock(mutex_);
    if (res < 0)
        throw std::exception();
}
