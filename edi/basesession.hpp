//
//  basesession.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef basesession_hpp
#define basesession_hpp

#include "noncopyable.hpp"
#include "ststreambuf.hpp"
#include "bufferediostream.h"
#include <memory>

class BaseSession: Noncopyable
{
public:
    virtual ~BaseSession();
    BufferedIOStream& io();
protected:
    BaseSession(st_utime_t timeout=60*UNIT);
    void set_istream(IStream& is);
    st_utime_t timeout_;
    void flush();
private:
    std::unique_ptr<STStreamBuf> inbuf_;
    std::unique_ptr<STStreamBuf> outbuf_;
    std::unique_ptr<BufferedIOStream> io_;
};
inline BufferedIOStream& BaseSession::io()
{
    return *io_;
}
inline void BaseSession::set_istream(IStream &is)
{
    inbuf_.reset(new STStreamBuf(is));
    outbuf_.reset(new STStreamBuf(is));
    io_.reset(new BufferedIOStream(*inbuf_,*outbuf_));
}

#endif /* basesession_hpp */
