//
// Created by lmz on 21/11/2017.
//

#include "bufferediostream.h"

BufferedIOStream::BufferedIOStream(STStreamBuf& in, STStreamBuf& out)
:std::istream(&in),std::ostream(&out),inbuf_(in),outbuf_(out)
{

}
