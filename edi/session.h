//
// Created by lmz on 21/11/2017.
//

#ifndef EDI_SESSION_H
#define EDI_SESSION_H

#include "socketstream.hpp"
#include "ststreambuf.hpp"
#include <string>
#include <istream>
#include <ostream>

class Session {
public:
    Session(const Session &) = delete;

    Session &operator=(const Session &)= delete;

    Session(const std::string &host, const std::string &port, st_utime_t timeout);

    void open();

    STStreamBuf *get_read_buf();

    STStreamBuf *get_write_buf();

private:
    std::string host_;
    std::string port_;
    SocketStream stSocket_;
    STStreamBuf inbuf_;
    STStreamBuf outbuf_;
    st_utime_t timeout_;
};

inline STStreamBuf *Session::get_read_buf() {
    return &inbuf_;
}

inline STStreamBuf *Session::get_write_buf() {
    return &outbuf_;
}


#endif //EDI_SESSION_H
