#pragma once
#include "basesession.hpp"
#include "noncopyable.hpp"
#include "socketstream.hpp"

class NetSession : public BaseSession {
public:
    NetSession(const std::string& host, const std::string& port, st_utime_t timeout = 60 * UNIT);
    ~NetSession();

private:
    void connect();

private:
    std::string host_;
    std::string port_;
    SocketStream ss_;
};
