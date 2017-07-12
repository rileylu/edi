//
// Created by Lmz on 12/07/2017.
//

#include "EventLoop.hpp"
#include "Channel.hpp"

Channel::Channel(EventLoop *loop, int fd) :
        loop_(loop),
        fd_(fd),
        events_(0),
        revents_(0),
        index_(-1) {

}

void Channel::handleEvent() {

}

void Channel::update() {
    loop_->updateChannel(this);
}
