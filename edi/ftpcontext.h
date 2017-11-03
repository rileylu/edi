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
	           const std::string& pwd);
	void SendFile(const std::string& fileName);
	void SendFile(std::shared_ptr<SyncQueue<std::string>> fileList);
	void RecvFile(const std::string& fileName);
	void RecvFile(std::shared_ptr<SyncQueue<std::string>> fileList);
	void List(const std::string& dir);

	std::shared_ptr<FtpSession> GetCtrlSession() const
	{
		return _ctrlSession;
	}

	std::shared_ptr<FtpSession> GetDataSession() const
	{
		return _dataSession;
	}

	void BuildDataSession(unsigned short port)
	{
		if (_dataSession)
			_dataSession->Close();
		_dataSession.reset(new FtpSession(_ios, _ip_address, port));
	}

	std::string GetUser() const
	{
		return _user;
	}

	std::string GetPWD() const
	{
		return _pwd;
	}

	std::string GetIP() const
	{
		return _ip_address;
	}

	unsigned short GetPort() const
	{
		return _port;
	}

	boost::asio::io_service& GetIOS() const
	{
		return _ios;
	}
	void Close() 
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

private:
	template <typename Fun>
	void ReBuild(Fun&& fun)
	{
		_ctrlSession->Timer().expires_from_now(boost::posix_time::seconds(30));
		_ctrlSession->Close();
		_ctrlSession.reset(new FtpSession(_ios, _ip_address, _port));
		if (_dataSession)
		{
			_dataSession->Close();
			_dataSession.reset();
		}
		ChangeStatus(&State::Instance());
		_ctrlSession->Timer().async_wait([fun](const boost::system::error_code& )
		{
			fun();
		});
	}

	void ChangeStatus(State* state)
	{
		_state = state;
	}

	bool WaitForTransfer()
	{
		std::unique_lock<std::mutex> lck(_m);
		bool result = _cv.wait_for(lck, std::chrono::seconds(1), [this]
		{
			return _ready_for_transfer;
		});
		if (result)
			_ready_for_transfer = false;
		return result;
	}

	void ReadyForTransfer()
	{
		std::lock_guard<std::mutex> lck(_m);
		_ready_for_transfer = true;
		_cv.notify_one();
	}

private:
	boost::asio::io_service& _ios;
	std::string _ip_address;
	unsigned short _port;
	std::string _user;
	std::string _pwd;
	std::shared_ptr<SyncQueue<std::string>> _fileList;


	friend class State;
	friend class EPSVReadyState;
	State* _state;
	std::shared_ptr<FtpSession> _ctrlSession;
	std::shared_ptr<FtpSession> _dataSession;

	std::mutex _m;
	bool _ready_for_transfer;
	std::condition_variable _cv;
};
