//
//  stsyncqueue.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef stsyncqueue_hpp
#define stsyncqueue_hpp

#include "noncopyable.hpp"
#include <vector>
#include <st.h>

template<typename T>
class STSyncQueue:Noncopyable
{
public:
    static const int max_size=10000;
    STSyncQueue(int max=max_size);
    void put(T&& t);
    void put(const T& t);
    void take(T& t);
    void emtpy();
    void full();
private:
    template<typename U>
    void take_(U&& u);
    
    bool full_;
    bool empty_;
    int max_;
    st_mutex_t mutex_;
    st_cond_t full_cond_;
    st_cond_t empty_cond_;
};

template<typename T>
STSyncQueue<T>::STSyncQueue(int maxsize)
:full_(false),empty_(true),max_(maxsize),mutex_(st_mutex_new()),full_cond_(st_cond_new()),empty_cond_(st_cond_new())
{
}
template<typename T>
template<typename U>
void STSyncQueue<T>::take_(U&& u)
{
    st_mutex_lock(mutex_);
}

#endif /* stsyncqueue_hpp */
