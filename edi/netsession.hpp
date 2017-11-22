//
//  netsession.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef netsession_hpp
#define netsession_hpp

#include "basesession.hpp"
#include "socketstream.hpp"
#include "noncopyable.hpp"

class NetSession:public BaseSession
{
public:
    NetSession(const std::string& host,const std::string& port,st_utime_t timeout=60*UNIT);
    ~NetSession();
private:
    void connect();
private:
    std::string host_;
    std::string port_;
    SocketStream ss_;
};


#endif /* netsession_hpp */
