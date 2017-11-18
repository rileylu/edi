#pragma once
#include "ftpcontext.h"
#include <memory>
class State {
public:
    void Run(std::shared_ptr<FtpContext> ftpContext);

protected:
    enum class Type {
        RETR,
        STOR,
        NLST
    };
    friend class FtpContext;
    State() = default;
    virtual ~State()=default;
    virtual Type GetType() const = 0;

    template <typename Fun>
    void parse_response(std::shared_ptr<FtpContext> ftpContext, const std::string& response, Fun&& f);

    void session_err(std::shared_ptr<FtpContext> ftpContext);
    void connect(std::shared_ptr<FtpContext> ftpContext);
    void user(std::shared_ptr<FtpContext> ftpContext);
    void pass(std::shared_ptr<FtpContext> ftpContext);
    void epsv(std::shared_ptr<FtpContext> ftpContext);
    void retr(std::shared_ptr<FtpContext> ftpContext);
    void stor(std::shared_ptr<FtpContext> ftpContext);
    void rnfr(std::shared_ptr<FtpContext> ftpContext);
    void rnto(std::shared_ptr<FtpContext> ftpContext);
    void nlst(std::shared_ptr<FtpContext> ftpContext);
    void logout(std::shared_ptr<FtpContext> ftpContext);
    void cwd(std::shared_ptr<FtpContext> ftpContext);
    void abor(std::shared_ptr<FtpContext> ftpContext);
    virtual void FileOP(std::shared_ptr<FtpContext> ftpContext) = 0;
};

class RetrState : public State {
public:
    static RetrState& Instance();

protected:
    virtual void FileOP(std::shared_ptr<FtpContext> ftpContext) override;
    virtual Type GetType() const override;
};

class StorState : public State {
public:
    static StorState& Instance();

protected:
    virtual Type GetType() const override;
    virtual void FileOP(std::shared_ptr<FtpContext> ftpContext) override;
};

class NlstState : public State {
public:
    static NlstState& Instance();

protected:
    virtual Type GetType() const override;
    virtual void FileOP(std::shared_ptr<FtpContext> ftpContext) override;
};

template <typename Fun>
inline void State::parse_response(std::shared_ptr<FtpContext> ftpContext, const std::string& response, Fun&& f)
{
    std::istream is(ftpContext->GetCtrlSession()->RecvBuf());
    std::string res;
    while (std::getline(is, res) && res[3] == '-')
        ;
    ftpContext->_res = std::move(res);
    if (ftpContext->_res.find(response) == 0) {
        f();
    } else if (ftpContext->_res.find("550") == 0) {
        if (response == "150") {
            ftpContext->_current_file.clear();
            epsv(ftpContext);
        } else if (response == "250") {
            ftpContext->Close();
            ftpContext.reset();
            return;
        }
    } else if (ftpContext->_res.find("530") == 0 && response == "230") {
        ftpContext->Close();
        ftpContext.reset();
        return;
    } else
        ftpContext->ReBuild(*this);
}
