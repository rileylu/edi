#pragma once

#include "guard.h"
#include "noncopyable.hpp"
#include "stcondition.h"
#include "stmutex.h"
#include <queue>

template<typename T>
class STSyncQueue : Noncopyable {
public:
    static const int max_size = 10000;
    
    STSyncQueue(int max = max_size);
    
    ~STSyncQueue();
    
    void put(T &&t);
    
    void put(const T &t);
    
    void take(T &t);
    
    bool empty();
    
    bool full();
    
private:
    template<typename F>
    void put_(F &&f);
    
    std::queue<T> data_;
    int max_;
    STMutex mutex_;
    STCondition notFull_;
    STCondition notEmpty_;
};

template<typename T>
template<typename F>
void STSyncQueue<T>::put_(F &&f) {
    while (data_.size() >= max_)
        notFull_.wait();
    Guard<STMutex> lck(mutex_);
    data_.push(std::forward<F>(f));
    notEmpty_.signal_one();
}

template<typename T>
STSyncQueue<T>::STSyncQueue(int maxsize)
: max_(maxsize) {
}

template<typename T>
STSyncQueue<T>::~STSyncQueue() {
    notEmpty_.signal_all();
    notFull_.signal_all();
}

template<typename T>
void STSyncQueue<T>::put(T &&t) {
    put_(t);
}

template<typename T>
void STSyncQueue<T>::put(const T &t) {
    put_(t);
}

template<typename T>
void STSyncQueue<T>::take(T &t) {
    while (data_.size() == 0)
        notEmpty_.wait();
    Guard<STMutex> lck(mutex_);
    t = std::move(data_.front());
    data_.pop();
    notFull_.signal_one();
}

template<typename T>
inline bool STSyncQueue<T>::empty() {
    Guard<STMutex> lck(mutex_);
    return data_.empty();
}

template<typename T>
bool STSyncQueue<T>::full() {
    Guard<STMutex> lck(mutex_);
    return data_.size() >= max_;
}
