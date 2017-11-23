//
//  netsession.cpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#include "netsession.hpp"
#include "utilities.hpp"

void NetSession::connect() {
    addrinfo addr=edi::getaddrinfo(host_, port_);
    int res=st_connect(ss_.get_des(), addr.ai_addr, addr.ai_addrlen, timeout_);
    if(res<0)
        throw std::exception();
}

NetSession::NetSession(const std::string &host, const std::string &port,st_utime_t timeout)
:BaseSession(timeout),host_(host),port_(port)
{
    connect();
    set_istream(ss_);
}

NetSession::~NetSession(){
    flush();
}


