//
// Created by lmz on 21/11/2017.
//

#include "sessionstream.h"

SessionStream::SessionStream(STSocketBuf *in, STSocketBuf *out)
:std::istream(in),std::ostream(out)
{

}
