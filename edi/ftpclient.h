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
#include "sessionstream.h"
#include "session.h"

class FTPClient {
public:
    const static int unit = 1000000;

    FTPClient(const std::string &host, const std::string &port, const std::string &user, const std::string &pass,
              st_utime_t timeout = 30 * unit);

    ~FTPClient() = default;

    void open();

    void login();

    void logout();

    void move_file(const std::string &fr, const std::string &to);

    void change_dir(const std::string& dir);

    std::istream& begin_download(const std::string& fn);
    void end_download();

    std::istream& begin_list(const std::string& dir);
    void end_list();

    std::ostream& begin_upload(const std::string& fn);
    void end_upload();



private:
    void parse_response(const std::string &res,const std::string& code);
    void prepare_datasession();

private:
    std::string host_;
    std::string port_;
    std::string user_;
    std::string pass_;
    st_utime_t timeout_;
    std::unique_ptr<Session> ftpCtrlSession_;
    std::unique_ptr<SessionStream> ctrlStream_;
    std::unique_ptr<Session> ftpDataSession_;
    std::unique_ptr<SessionStream> dataStream_;
};


#endif //EDI_FTPCLIENT_H
