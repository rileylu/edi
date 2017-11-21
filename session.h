//
// Created by lmz on 21/11/2017.
//

#ifndef EDI_SESSION_H
#define EDI_SESSION_H

#include "stsocket.hpp"
#include "stsocketbuf.hpp"
#include <string>
#include <istream>
#include <ostream>

class Session {
public:
    Session(const Session &) = delete;

    Session &operator=(const Session &)= delete;

    Session(const std::string &host, const std::string &port, st_utime_t timeout);

    void open();

    std::istream &read_data();

    std::ostream &write_data();

private:

    std::string host_;
    std::string port_;

    STSocket stSocket_;
    STSocketBuf inbuf_;
    std::istream is_;
    STSocketBuf outbuf_;
    std::ostream os_;
    st_utime_t timeout_;
};


#endif //EDI_SESSION_H
