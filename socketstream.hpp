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
    ssize_t read(void *buf, size_t len, st_utime_t timeout=60*UNIT) override;
    ssize_t write(const void *buf, size_t len,st_utime_t timeout=60*UNIT) override;
    st_netfd_t get_des() const override;
private:
    static void destructor(void *args);
    st_netfd_t des_;
};

inline st_netfd_t SocketStream::get_des() const
{
    return des_;
}

#endif /* socketstream_hpp */
