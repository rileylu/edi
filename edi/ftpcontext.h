#pragma once
#include <memory>
#include <string>
#include "ftpsession.h"
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <utility>

template <typename T>
class SyncQueue;
class State;

class FtpContext : public std::enable_shared_from_this<FtpContext>
{
public:
	FtpContext(boost::asio::io_service& ios, const std::string& ip, unsigned short port, const std::string& user,
		const std::string& pwd, const std::string& dir, std::shared_ptr<SyncQueue<std::string>> fileList);

	void SendFile();

	void RecvFile();

	void List();

	std::shared_ptr<FtpSession> GetCtrlSession() const;

	std::shared_ptr<FtpSession> GetDataSession() const;

	std::string GetUser() const;

	std::string GetPWD() const;

	std::string GetIP() const;

	unsigned short GetPort() const;

	boost::asio::io_service& GetIOS() const;

	void Close();

private:
	void BuildDataSession(unsigned short port);

	void ReBuild(State &s);
private:

	boost::asio::io_service& _ios;
	std::string _ip_address;
	unsigned short _port;
	std::string _user;
	std::string _pwd;
	std::string _dir;
	std::string _current_file;
	std::string _res;
	std::shared_ptr<SyncQueue<std::string>> _fileList;

	friend class State;
	friend class RecvState;
	friend class StorState;
	friend class NlstState;
	std::shared_ptr<FtpSession> _ctrlSession;
	std::shared_ptr<FtpSession> _dataSession;
};

inline std::shared_ptr<FtpSession> FtpContext::GetCtrlSession() const
{
	return _ctrlSession;
}

inline std::shared_ptr<FtpSession> FtpContext::GetDataSession() const
{
	return _dataSession;
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

inline boost::asio::io_service & FtpContext::GetIOS() const
{
	return _ios;
}

inline void FtpContext::Close()
{
	if (_ctrlSession)
	{
		_ctrlSession->Close();
		_ctrlSession.reset();
	}
	if (_dataSession)
	{
		_dataSession->Close();
		_dataSession.reset();
	}
}

inline void FtpContext::BuildDataSession(unsigned short port)
{
	if (_dataSession)
		_dataSession->Close();
	_dataSession.reset(new FtpSession(_ios, _ip_address, port));
}
