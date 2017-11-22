//
//  istream.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef istream_hpp
#define istream_hpp

#include <sys/types.h>
#include <st.h>
#define UNIT 1000000

class IStream
{
public:
    virtual ~IStream()=default;
    virtual ssize_t read(void *buf,size_t len)=0;
    virtual ssize_t write(const void* buf,size_t len)=0;
    void set_timeout(st_utime_t timeout);
    st_utime_t get_timeout() const ;
protected:
    IStream(st_utime_t timeout=60*UNIT);
    st_utime_t timeout_;
};

inline void IStream::set_timeout(st_utime_t timeout)
{
    timeout_=timeout;
}
inline st_utime_t IStream::get_timeout() const
{
    return timeout_;
}


#endif /* istream_hpp */
