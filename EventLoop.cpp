//
// Created by Lmz on 12/07/2017.
//

#include <cstdlib>
#include <cassert>
#include "EventLoop.hpp"

thread_local EventLoop *t_loopInThisThread = 0;

EventLoop::EventLoop() :
        looping_(false),
        threadId_(std::this_thread::get_id()) {
    //log
    if (t_loopInThisThread) {
        //log
        exit(1);
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    //log
    looping_ = false;
}

void EventLoop::abortNotInLoopThread() {

}

void EventLoop::updateChannel(Channel *) {

}
