//
//  filestream.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef filestream_hpp
#define filestream_hpp

#include "istream.hpp"
#include "noncopyable.hpp"
#include <fcntl.h>
#include <st.h>

class FileStream: public IStream,Noncopyable
{
public:
    FileStream();
    virtual ~FileStream();
    ssize_t read(void *buf, size_t len,st_utime_t timeout=60*UNIT) override;
    ssize_t write(const void *buf, size_t len,st_utime_t timeout=60*UNIT) override;
    st_netfd_t get_des() const override;
    void set_des(st_netfd_t des);
private:
    st_netfd_t des_;
};

inline st_netfd_t FileStream::get_des() const
{
    return des_;
}
inline void FileStream::set_des(st_netfd_t des)
{
    des_=des;
}

#endif /* filestream_hpp */
