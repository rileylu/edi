//
// Created by Lmz on 12/07/2017.
//

#ifndef EDI_EVENTLOOP_HPP
#define EDI_EVENTLOOP_HPP

#include "Noncopyable.hpp"
#include <atomic>
#include <thread>

class EventLoop : Noncopyable {
public:
    EventLoop();

    ~EventLoop();

    void loop();

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    static EventLoop *getEventLoopOfCurrentThread();

    bool isInLoopThread() const {
        return threadId_ == std::this_thread::get_id();
    }

private:
    void abortNotInLoopThread();

    std::atomic_bool looping_;
    std::thread::id threadId_;


};


#endif //EDI_EVENTLOOP_HPP
