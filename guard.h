#pragma once
#include "noncopyable.hpp"
template <typename T>
class Guard : Noncopyable {
public:
    Guard(T& t);
    ~Guard();

private:
    T& guarded_;
};

template <typename T>
Guard<T>::Guard(T& t)
    : guarded_(t)
{
    guarded_.lock();
}

template <typename T>
Guard<T>::~Guard() { guarded_.unlock(); }
