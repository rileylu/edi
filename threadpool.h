#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "stsyncqueue.hpp"
#include <functional>
#include <list>

class ThreadPool {
public:
    using Task = std::function<void()>;
    ThreadPool(int max_tds);
    ~ThreadPool();
    void stop();
    void add_task(Task&& task);
    void add_task(const Task& task);

private:
    void start();
    static void* run_in_thread(void*);

private:
    std::list<st_thread_t> tds_;
    STSyncQueue<Task> task_list_;
    bool running_;
    int max_tds_;
};

#endif // THREADPOOL_H
