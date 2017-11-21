//
// Created by lmz on 21/11/2017.
//

#ifndef EDI_FTPCLIENT_H
#define EDI_FTPCLIENT_H

#include <string>
#include <istream>
#include <ostream>
#include <memory>
#include <st.h>

class Session;

class FTPClient {
public:
    const static int unit = 1000000;

    FTPClient(const std::string &host, const std::string &port, const std::string &user, const std::string &pass,
              st_utime_t timeout = 30 * unit);

    ~FTPClient() = default;

    void open();

    void login();

    std::istream &get_list(const std::string &dir);

    std::istream &download_file(const std::string &fn);

    std::ostream &upload_file(const std::string &fn);

private:
    void parse_response();

private:
    std::string host_;
    std::string port_;
    std::string user_;
    std::string pass_;
    st_utime_t timeout_;
    std::unique_ptr<Session> ftpCtrlSession_;
    std::unique_ptr<Session> ftpDataSession_;
};


#endif //EDI_FTPCLIENT_H
