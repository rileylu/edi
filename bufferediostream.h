#pragma once
#include "ststreambuf.hpp"
#include <istream>
#include <ostream>
class BufferedIOStream : public std::istream, public std::ostream {
public:
    BufferedIOStream(STStreamBuf& in, STStreamBuf& out);
    STStreamBuf& get_in_buf();
    STStreamBuf& get_out_buf();

private:
    STStreamBuf& inbuf_;
    STStreamBuf& outbuf_;
};
