#include "state.h"
#include "threadsafe_queue.h"
#include <iostream>
#include <regex>
#include <fstream>

using namespace boost;

void State::Run(std::shared_ptr<FtpContext> ftpContext) {
    connect(ftpContext);
}

void State::session_err(std::shared_ptr<FtpContext> ftpContext) {
    if (ftpContext->_fileList->empty() && ftpContext->_current_file.size() == 0) {
        ftpContext->Close();
        ftpContext.reset();
    } else
        ftpContext->ReBuild(*this);
}

inline void State::connect(std::shared_ptr<FtpContext> ftpContext) {
    ftpContext->GetCtrlSession()->async_connect([this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            parse_response(ftpContext, "220", std::bind(&State::user, this, ftpContext));
        },
        std::bind(&State::session_err, this, ftpContext));

    },
    std::bind(&State::session_err, this, ftpContext));
}

inline void State::user(std::shared_ptr<FtpContext> ftpContext) {
    std::string cmd;
    cmd += "USER ";
    cmd += ftpContext->GetUser();
    cmd += "\r\n";
    ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            parse_response(ftpContext, "331", std::bind(&State::pass, this, ftpContext));
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

inline void State::pass(std::shared_ptr<FtpContext> ftpContext) {
    std::string cmd;
    cmd += "PASS ";
    cmd += ftpContext->GetPWD();
    cmd += "\r\n";
    ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            parse_response(ftpContext, "230", std::bind(&State::cwd, this, ftpContext));
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

inline void State::epsv(std::shared_ptr<FtpContext> ftpContext) {
    ftpContext->GetCtrlSession()->async_send("EPSV\r\n", [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            auto fun = [this, ftpContext] {
                std::regex port_regex(R"(.*\|{3}([0-9]+)\|{1}.*)");
                std::smatch results;
                unsigned short port = 0;
                if (std::regex_search(ftpContext->_res, results, port_regex)) {
                    port = std::stoul(std::string(results[1].first, results[1].second));
                    ftpContext->BuildDataSession(port);
                    ftpContext->GetDataSession()->async_connect([this, ftpContext] { FileOP(ftpContext); },
                    [this, ftpContext] {
                        if (ftpContext->GetCtrlSession()->Err())
                            return;
                        else
                            epsv(ftpContext);
                    });
                } else
                    epsv(ftpContext);
            };
            parse_response(ftpContext, "229", fun);
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

void State::retr(std::shared_ptr<FtpContext> ftpContext) {
    if (ftpContext->_current_file.size() == 0) {
        if (!ftpContext->_fileList->try_pop(ftpContext->_current_file)) {
            logout(ftpContext);
            return;
        }
    }
    std::string cmd = "RETR ";
    cmd += ftpContext->_current_file;
    cmd += "\r\n";
    ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            auto data = [this, ftpContext] {
                ftpContext->GetDataSession()->async_read([] {}, [this, ftpContext] {
                    auto err = ftpContext->GetDataSession()->Err();
                    if (err.value() == 2) {
                        std::ofstream of(ftpContext->_current_file.c_str(), std::ios::binary);
                        if (of.is_open()) {
                            auto buf = ftpContext->GetDataSession()->GetSharedRecvBuf();
                            of << buf;
                            of.flush();
                            of.close();
                            ftpContext->GetDataSession()->Close();
                            ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
                                parse_response(ftpContext, "226", [this, ftpContext] {
                                    ::fprintf(stdout, "Tranmit File: %s successfully.\n",
                                              ftpContext->_current_file.c_str());
                                    rnfr(ftpContext);
                                });
                            }, std::bind(&State::session_err, this, ftpContext));
                        } else
                            epsv(ftpContext);
                    } else if (ftpContext->GetCtrlSession()->Err())
                        return;
                    else
                        epsv(ftpContext);
                });
            };
            parse_response(ftpContext, "150", data);
        }, std::bind(&State::session_err, this, ftpContext));
    }, std::bind(&State::session_err, this, ftpContext));
}

void State::stor(std::shared_ptr<FtpContext> ftpContext) {
    if (ftpContext->_current_file.size() == 0) {
        if (!ftpContext->_fileList->try_pop(ftpContext->_current_file)) {
            logout(ftpContext);
            return;
        }
    }
    std::ifstream fd(ftpContext->_current_file.c_str(), std::ios::binary);
    if (fd.is_open()) {
        auto buf = std::make_unique<boost::asio::streambuf>();
        while(!fd.eof()&&fd>>buf.get());
        fd.close();
        ftpContext->GetDataSession()->RecvBuf(buf.release());
        std::string cmd;
        cmd += "STOR ";
        cmd += "tmp/" + ftpContext->_current_file;
        cmd += "\r\n";
        ftpContext->GetCtrlSession()->async_send(cmd, [ftpContext, this] {
            ftpContext->GetCtrlSession()->async_readuntil("\r\n", [ftpContext, this] {
                parse_response(ftpContext, "150", [this, ftpContext] {
                    ftpContext->GetDataSession()->async_send([this, ftpContext] {
                        ftpContext->GetDataSession()->Close();
                        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
                            parse_response(ftpContext, "226", [this, ftpContext] {
                                rnfr(ftpContext);
                            });
                        }, std::bind(&State::session_err, this, ftpContext));
                    }, std::bind(&State::session_err, this, ftpContext));
                });
            }, std::bind(&State::session_err, this, ftpContext));
        }, std::bind(&State::session_err, this, ftpContext));
    } else {
        ftpContext->_current_file.clear();
        stor(ftpContext);
    }

}

void State::rnfr(std::shared_ptr<FtpContext> ftpContext) {
    std::string cmd;
    cmd += "RNFR ";
    switch (GetType()) {
    case Type::NLST:
        logout(ftpContext);
        return;
    case Type::RETR:
        cmd += ftpContext->_current_file;
        break;
    case Type::STOR:
        cmd += "tmp/" + ftpContext->_current_file;
        break;
    }
    cmd += "\r\n";
    ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            auto fun = [this, ftpContext] {
                rnto(ftpContext);
            };
            parse_response(ftpContext, "350", fun);
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

void State::rnto(std::shared_ptr<FtpContext> ftpContext) {
    std::string cmd;
    cmd += "RNTO ";
    switch (GetType()) {
    case Type::RETR:
        cmd += "bak/" + ftpContext->_current_file;
        break;
    case Type::STOR:
        cmd += ftpContext->_current_file;
        break;
    case Type::NLST:
        break;
    }
    cmd += "\r\n";
    ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            auto fun = [this, ftpContext] {
                if (ftpContext->_fileList->try_pop(ftpContext->_current_file))
                    epsv(ftpContext);
                else
                    logout(ftpContext);
            };
            parse_response(ftpContext, "250", fun);
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

void State::nlst(std::shared_ptr<FtpContext> ftpContext) {
}

void State::logout(std::shared_ptr<FtpContext> ftpContext) {
    ftpContext->GetCtrlSession()->async_send("QUIT\r\n", [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            parse_response(ftpContext, "221", [this, ftpContext]() mutable {
                ftpContext->Close();
                ftpContext.reset();
            });
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

void State::cwd(std::shared_ptr<FtpContext> ftpContext) {
    std::string cmd;
    cmd += "CWD ";
    cmd += ftpContext->_dir;
    cmd += "\r\n";
    ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            parse_response(ftpContext, "250", std::bind(&State::epsv, this, ftpContext));
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

void State::abor(std::shared_ptr<FtpContext> ftpContext) {
    ftpContext->GetCtrlSession()->async_send("ABOR\r\n", [this, ftpContext] {
        ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
            parse_response(ftpContext, "226", [this, ftpContext] {
                epsv(ftpContext);
            });
        },
        std::bind(&State::session_err, this, ftpContext));
    },
    std::bind(&State::session_err, this, ftpContext));
}

RetrState &RetrState::Instance() {
    static RetrState recvState;
    return recvState;
}

void RetrState::FileOP(std::shared_ptr<FtpContext> ftpContext) {
    retr(ftpContext);
}

State::Type RetrState::GetType() const {
    return State::Type::RETR;
}

StorState &StorState::Instance() {
    static StorState storState;
    return storState;
}

State::Type StorState::GetType() const {
    return Type::STOR;
}

void StorState::FileOP(std::shared_ptr<FtpContext> ftpContext) {
    stor(ftpContext);
}

NlstState &NlstState::Instance() {
    static NlstState nlstState;
    return nlstState;
}

State::Type NlstState::GetType() const {
    return State::Type::NLST;
}

void NlstState::FileOP(std::shared_ptr<FtpContext> ftpContext) {
    nlst(ftpContext);
}
