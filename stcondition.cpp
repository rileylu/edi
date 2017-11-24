#include "stcondition.h"

STCondition::STCondition()
    : cond_(nullptr)
{
    cond_ = st_cond_new();
    if (cond_ == nullptr)
        throw std::exception();
}

STCondition::~STCondition()
{
    st_cond_destroy(cond_);
}

void STCondition::signal_one()
{
    st_cond_signal(cond_);
}

void STCondition::signal_all()
{
    st_cond_broadcast(cond_);
}

void STCondition::wait()
{
    int res = 0;
    do {
        res = st_cond_wait(cond_);
    } while (res < 0 && errno == EINTR);
}
