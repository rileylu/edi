#pragma once
#include "socketstream.hpp"
#include <istream>
#include <memory>
#include <ostream>
#include <st.h>
#include <string>

class FTPClient {
public:
    FTPClient(const std::string& host, const std::string& port, const std::string& user, const std::string& pass,
        st_utime_t timeout = 10 * edi::TIMEOUT_UNIT);

    ~FTPClient();

    void open();

    void login();

    void logout();

    void move_file(const std::string& fr, const std::string& to);

    void change_dir(const std::string& dir);

    std::istream& begin_download(const std::string& fn);
    void end_download();

    std::istream& begin_list(const std::string& dir);
    void end_list();

    std::ostream& begin_upload(const std::string& fn);
    void end_upload();

private:
    void parse_response(const std::string& res, const std::string& code);
    void prepare_datasession();

private:
    std::string host_;
    std::string port_;
    std::string user_;
    std::string pass_;
    st_utime_t timeout_;

    SocketStream ctrlSession_;
    std::unique_ptr<SocketStream> dataSession_;
};
