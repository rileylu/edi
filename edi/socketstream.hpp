//
//  socketstream.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef socketstream_hpp
#define socketstream_hpp

#include "istream.hpp"
#include "noncopyable.hpp"
#include <st.h>

class SocketStream:public IStream,Noncopyable
{
public:
    SocketStream();
    virtual ~SocketStream();
    ssize_t read(void *buf, size_t len) override;
    ssize_t write(const void *buf, size_t len) override;
    int connect(sockaddr *addr,int addrlen);
private:
    static void destructor(void *args);
    st_netfd_t des_;
};

#endif /* socketstream_hpp */
