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
    virtual ssize_t read(void *buf,size_t len, st_utime_t timeout=60*UNIT)=0;
    virtual ssize_t write(const void* buf,size_t len,st_utime_t timeout=60*UNIT)=0;
    virtual st_netfd_t get_des() const=0;
protected:
    IStream()=default;
};
#endif /* istream_hpp */
