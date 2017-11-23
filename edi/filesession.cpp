//
//  filesession.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "filesession.hpp"


FileSession::FileSession(const std::string &fn, int flags, mode_t mode,st_utime_t timeout)
:BaseSession(timeout)
{
    auto fd=st_open(fn.c_str(), flags,mode);
    if(fd==nullptr)
        throw std::exception();
    fs_.set_des(fd);
    set_istream(fs_);
}

FileSession::~FileSession(){
    flush();
}


