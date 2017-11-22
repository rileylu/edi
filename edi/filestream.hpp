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
    ssize_t read(void *buf, size_t len) override;
    ssize_t write(const void *buf, size_t len) override;
    void open(const char* fn,int flag,int mode);
private:
    st_netfd_t des_;
};

#endif /* filestream_hpp */
