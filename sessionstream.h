//
// Created by lmz on 21/11/2017.
//

#ifndef EDI_SESSIONSTREAM_H
#define EDI_SESSIONSTREAM_H

#include <istream>
#include <ostream>
#include "stsocketbuf.hpp"

class SessionStream:public std::istream,public std::ostream{
public:
    SessionStream(STSocketBuf* in,STSocketBuf* out);

};


#endif //EDI_SESSIONSTREAM_H
