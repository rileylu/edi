//
// Created by lmz on 21/11/2017.
//

#ifndef EDI_SESSIONSTREAM_H
#define EDI_SESSIONSTREAM_H

#include <istream>
#include <ostream>
#include "ststreambuf.hpp"
class BufferedIOStream:public std::istream,public std::ostream{
public:
    BufferedIOStream(STStreamBuf& in,STStreamBuf& out);
    STStreamBuf& get_in_buf();
    STStreamBuf& get_out_buf();
private:
    STStreamBuf& inbuf_;
    STStreamBuf& outbuf_;
};

inline STStreamBuf& BufferedIOStream::get_in_buf()
{
    return inbuf_;
}

inline STStreamBuf& BufferedIOStream::get_out_buf()
{
    return outbuf_;
}


#endif //EDI_SESSIONSTREAM_H
