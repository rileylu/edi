#pragma once
#include "ftpsession.h"
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

template <typename T>
class threadsafe_queue;
class State;

class FtpContext : public std::enable_shared_from_this<FtpContext> {
public:
    FtpContext(boost::asio::io_service& ios, const std::string& ip, unsigned short port, const std::string& user,
        const std::string& pwd, const std::string& dir, std::shared_ptr<threadsafe_queue<std::string> > fileList);

    void SendFile();

    void RecvFile();

    void List();

    const std::unique_ptr<FtpSession>& GetCtrlSession() const;

    const std::unique_ptr<FtpSession>& GetDataSession() const;

    void ResetCtrlSession();
    void ResetDataSession();

    std::string GetUser() const;

    std::string GetPWD() const;

    std::string GetIP() const;

    unsigned short GetPort() const;

    boost::asio::io_service& GetIOS() const;

    void Close();

    std::string GetDir() const;

private:
    void BuildDataSession(unsigned short port);

    void ReBuild(State& s);

private:
    boost::asio::io_service& _ios;
    std::string _ip_address;
    unsigned short _port;
    std::string _user;
    std::string _pwd;
    std::string _dir;
    std::string _current_file;
    std::string _res;
    std::shared_ptr<threadsafe_queue<std::string> > _fileList;

    friend class State;
    friend class RetrState;
    friend class StorState;
    friend class NlstState;
    std::unique_ptr<FtpSession> _ctrlSession;
    std::unique_ptr<FtpSession> _dataSession;

};

inline const std::unique_ptr<FtpSession>& FtpContext::GetCtrlSession() const
{
    return _ctrlSession;
}

inline const std::unique_ptr<FtpSession>& FtpContext::GetDataSession() const
{
    return _dataSession;
}

inline void FtpContext::ResetCtrlSession()
{
    _ctrlSession.reset(nullptr);
}
inline void FtpContext::ResetDataSession()
{
    _dataSession.reset(nullptr);
}

inline std::string FtpContext::GetUser() const
{
    return _user;
}

inline std::string FtpContext::GetPWD() const
{
    return _pwd;
}

inline std::string FtpContext::GetIP() const
{
    return _ip_address;
}

inline unsigned short FtpContext::GetPort() const
{
    return _port;
}

inline boost::asio::io_service& FtpContext::GetIOS() const
{
    return _ios;
}

inline void FtpContext::Close()
{
    if (_ctrlSession) {
        _ctrlSession->Close();
        _ctrlSession.reset(nullptr);
    }
    if (_dataSession) {
        _dataSession->Close();
        _dataSession.reset(nullptr);
    }
}

inline std::string FtpContext::GetDir() const
{
    return _dir;
}

inline void FtpContext::BuildDataSession(unsigned short port)
{
    auto t = std::make_unique<FtpSession>(_ios, _ip_address, port);
    if (_dataSession) {
        _dataSession->Close();
    }
    _dataSession.reset(t.release());
}
