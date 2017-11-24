#include "threadpool.h"

ThreadPool::ThreadPool(int max_tds)
    : max_tds_(max_tds)
{
    start();
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::stop()
{
    task_list_.stop();
    running_ = false;
    for (auto td : tds_) {
        st_thread_join(td, 0);
    }
    tds_.clear();
}

void ThreadPool::add_task(ThreadPool::Task&& task)
{
    task_list_.put(std::move(task));
}

void ThreadPool::add_task(const ThreadPool::Task& task)
{
    task_list_.put(task);
}

void ThreadPool::start()
{
    running_ = true;
    for (int i = 0; i < max_tds_; ++i) {
        auto td = st_thread_create(&ThreadPool::run_in_thread, this, 1, 0);
        tds_.push_back(td);
    }
}

void* ThreadPool::run_in_thread(void* arg)
{
    ThreadPool* p = reinterpret_cast<ThreadPool*>(arg);
    while (p->running_) {
        Task task;
        p->task_list_.take(task);
        if (p->running_)
            task();
    }
    return 0;
}
