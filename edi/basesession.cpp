//
//  basesession.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "basesession.hpp"


BaseSession::BaseSession(st_utime_t timeout)
:inbuf_(nullptr),outbuf_(nullptr),io_(nullptr),timeout_(timeout)
{
}

BaseSession::~BaseSession() {
    if(inbuf_)
        inbuf_.reset(nullptr);
    if(outbuf_)
        outbuf_.reset(nullptr);
    if(io_)
        io_.reset(nullptr);
}

void BaseSession::flush() {
    io_->sync();
}


