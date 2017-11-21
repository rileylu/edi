//
// Created by lmz on 21/11/2017.
//

#include "ftpclient.h"
#include "session.h"

FTPClient::FTPClient(const std::string &host, const std::string &port, const std::string &user,
                     const std::string &pass,st_utime_t timeout)
:host_(host),port_(port),user_(user),pass_(pass),timeout_(timeout)
{
    ftpCtrlSession_.reset(new Session(host_,port_,timeout_));
}

void FTPClient::open() {
    ftpCtrlSession_->open();
    std::string line;
    std::istream& is=ftpCtrlSession_->read_data();
    do
    {
        std::getline(is,line);
    }while(line[3]=='-');
}

void FTPClient::login() {
    std::string cmd;
    cmd+="USER ";
    cmd+=user_;
    cmd+="\r\n";
    cmd+="PASS ";
    cmd+=pass_;
    cmd+="\r\n";
    std::ostream& os=ftpCtrlSession_->write_data();
    os.write(cmd.c_str(),cmd.size());
    std::istream& is=ftpCtrlSession_->read_data();

}
