#pragma once
#include "basesession.hpp"
#include "filestream.hpp"

class FileSession : public BaseSession {
public:
    FileSession(const std::string& fn, int flags, mode_t mode = 0755, st_utime_t timeout = 60 * UNIT);
    ~FileSession();

private:
    FileStream fs_;
};
