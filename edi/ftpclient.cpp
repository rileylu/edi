//
// Created by lmz on 21/11/2017.
//

#include "ftpclient.h"
#include <regex>

FTPClient::FTPClient(const std::string& host, const std::string& port, const std::string& user,
    const std::string& pass, st_utime_t timeout)
    : host_(host)
    , port_(port)
    , user_(user)
    , pass_(pass)
    , timeout_(timeout)
    , ctrlSession_(host, port, timeout)
    , dataSession_(nullptr)
{
}

void FTPClient::open()
{
    std::string line;
    do {
        std::getline(ctrlSession_, line);
    } while (line[3] == '-');
    parse_response(line, "220");
}

void FTPClient::login()
{
    std::string cmd;
    cmd += "USER ";
    cmd += user_;
    cmd += "\r\n";
    cmd += "PASS ";
    cmd += pass_;
    cmd += "\r\n";
    std::string line;
    ctrlSession_ << cmd << std::flush;
    std::getline(ctrlSession_, cmd);
    parse_response(cmd, "331");
    std::getline(ctrlSession_, cmd);
    parse_response(cmd, "230");
}

void FTPClient::parse_response(const std::string& res, const std::string& code)
{
    if (res.find(code) == res.npos) {
        std::fprintf(stderr, "%s\n", res);
        throw std::exception();
    }
}

void FTPClient::logout()
{
    ctrlSession_ << "QUIT\r\n"
                 << std::flush;
    std::string line;
    std::getline(ctrlSession_, line);
    parse_response(line, "221");
}

void FTPClient::move_file(const std::string& fr, const std::string& to)
{
    std::string cmd = "RNFR ";
    cmd += fr;
    cmd += "\r\n";
    std::string line;
    std::getline(ctrlSession_, line);
    parse_response(line, "");
    cmd = "RNTO ";
    cmd += to;
    cmd += "\r\n";
    std::getline(ctrlSession_, line);
    parse_response(line, "");
}

std::istream& FTPClient::begin_download(const std::string& fn)
{
    prepare_datasession();
    std::string cmd = "RETR ";
    cmd += fn;
    cmd += "\r\n";
    ctrlSession_ << cmd << std::flush;
    return *dataSession_;
}

void FTPClient::end_download()
{
    std::string res;
    std::getline(ctrlSession_, res);
    parse_response(res, "150");
    std::getline(ctrlSession_, res);
    parse_response(res, "226");
    dataSession_.reset(nullptr);
}

std::istream& FTPClient::begin_list(const std::string& dir)
{
    prepare_datasession();
    std::string cmd = "NLST ";
    cmd += dir;
    cmd += "\r\n";
    ctrlSession_ << cmd << std::flush;
    return *dataSession_;
}

void FTPClient::end_list()
{
    std::string res;
    std::getline(ctrlSession_, res);
    parse_response(res, "150");
    std::getline(ctrlSession_, res);
    parse_response(res, "226");
    dataSession_.reset(nullptr);
}

std::ostream& FTPClient::begin_upload(const std::string& fn)
{
    prepare_datasession();
    std::string cmd = "STOR ";
    cmd += fn;
    cmd += "\r\n";
    ctrlSession_ << cmd << std::flush;
    std::string res;
    std::getline(ctrlSession_, res);
    parse_response(res, "150");
    return *dataSession_;
}

void FTPClient::end_upload()
{
    dataSession_.reset(nullptr);
    std::string res;
    std::getline(ctrlSession_, res);
    parse_response(res, "226");
}

void FTPClient::prepare_datasession()
{
    std::string cmd = "EPSV\r\n";
    std::string res;
    ctrlSession_ << cmd << std::flush;
    std::getline(ctrlSession_, res);
    parse_response(res, "229");
    //performance
    std::regex port_regex(R"(.*\|{3}([0-9]+)\|{1}.*)");
    std::smatch results;
    if (std::regex_search(res, results, port_regex)) {
        dataSession_.reset(new SocketStream(host_, results[1], timeout_));
    } else
        throw std::exception();
}

void FTPClient::change_dir(const std::string& dir)
{
    std::string cmd = "CWD ";
    cmd += dir;
    cmd += "\r\n";
    ctrlSession_ << cmd << std::flush;
    std::string res;
    std::getline(ctrlSession_, res);
    parse_response(res, "250");
}

FTPClient::~FTPClient()
{
    if (dataSession_)
        dataSession_.reset(nullptr);
}
