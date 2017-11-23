#pragma once
#include "noncopyable.hpp"
#include <exception>
#include <st.h>

class STCondition : Noncopyable {
public:
    STCondition();
    ~STCondition();
    void signal_one();
    void signal_all();
    void wait();

private:
    st_cond_t cond_;
};
