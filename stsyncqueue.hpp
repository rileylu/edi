#pragma once

#include "guard.h"
#include "noncopyable.hpp"
#include "stcondition.h"
#include "stmutex.h"
#include <list>

template <typename T>
class STSyncQueue : Noncopyable {
public:
    STSyncQueue(int max = 10000);

    ~STSyncQueue();

    void put(T&& t);

    void put(const T& t);

    void take(T& t);

    void take(std::list<T>& l);

    bool empty();

    bool full();

    void stop();

private:
    template <typename F>
    void put_(F&& f);

    std::list<T> data_;
    int max_;
    STMutex mutex_;
    STCondition notFull_;
    STCondition notEmpty_;
    bool need_stop_;
};

template <typename T>
template <typename F>
void STSyncQueue<T>::put_(F&& f)
{
    while (data_.size() >= max_ && !need_stop_)
        notFull_.wait();
    Guard<STMutex> lck(mutex_);
    if (need_stop_)
        return;
    data_.push_back(std::forward<F>(f));
    notEmpty_.signal_one();
}

template <typename T>
STSyncQueue<T>::STSyncQueue(int maxsize)
    : max_(maxsize)
    , need_stop_(false)
{
}

template <typename T>
STSyncQueue<T>::~STSyncQueue()
{
    notEmpty_.signal_all();
    notFull_.signal_all();
}

template <typename T>
void STSyncQueue<T>::put(T&& t)
{
    put_(std::move(t));
}

template <typename T>
void STSyncQueue<T>::put(const T& t)
{
    put_(t);
}

template <typename T>
void STSyncQueue<T>::take(T& t)
{
    while (data_.size() == 0 && !need_stop_)
        notEmpty_.wait();
    Guard<STMutex> lck(mutex_);
    if (need_stop_)
        return;
    t = std::move(data_.front());
    data_.pop_front();
    notFull_.signal_one();
}

template <typename T>
void STSyncQueue<T>::take(std::list<T>& l)
{
    while (data_.size() == 0 && !need_stop_)
        notEmpty_.wait();
    Guard<STMutex> lck(mutex_);
    if (need_stop_)
        return;
    l = std::move(data_);
    notFull_.signal_one();
}

template <typename T>
inline bool STSyncQueue<T>::empty()
{
    Guard<STMutex> lck(mutex_);
    return data_.empty();
}

template <typename T>
bool STSyncQueue<T>::full()
{
    Guard<STMutex> lck(mutex_);
    return data_.size() >= max_;
}

template <typename T>
void STSyncQueue<T>::stop()
{
    Guard<STMutex> lck(mutex_);
    need_stop_ = true;
    notEmpty_.signal_all();
    notFull_.signal_all();
}
