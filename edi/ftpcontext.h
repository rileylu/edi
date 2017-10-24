#pragma once
#include <memory>
#include <string>
#include "session.h"
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <boost/lockfree/queue.hpp>

template<typename T>
class threadsafe_queue;
class State;
class FtpContext :public std::enable_shared_from_this<FtpContext> {
public:
	FtpContext(boost::asio::io_service& ios, const std::string& ip, unsigned short port, const std::string& user, const std::string& pwd);
	void SendFile(const std::string& fileName);
	void SendFile(std::shared_ptr<threadsafe_queue<std::string>> fileList);
	void RecvFile(const std::string& fileName);
	void List(const std::string& dir);
	std::shared_ptr<Session> GetCtrlSession() const {
		return _ctrlSession;
	}
	std::shared_ptr<Session> GetDataSession() const {
		return _dataSession;
	}

	void BuildDataSession(unsigned short port) {
		_dataSession.reset(new Session(_ios, _ip_address, port));
	}
	std::string GetUser() const {
		return _user;
	}
	std::string GetPWD() const {
		return _pwd;
	}
	std::string GetIP() const {
		return _ip_address;
	}
	unsigned short GetPort() const {
		return _port;
	}

	boost::asio::io_service& GetIOS() const {
		return _ios;
	}
private:
	void ReBuild();
	void ChangeState(State* s) {
		_state = s;
	}
	void DoSendFile(const std::string& filename);
	bool WaitForTransfer() {
		std::unique_lock<std::mutex> lck(_m);
		bool result = _cv.wait_for(lck, std::chrono::seconds(1), [this] {
			return _ready_for_transfer;
		});
		if (result)
			_ready_for_transfer = false;
		return result;
	}
	void ReadyForTransfer() {
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
	std::shared_ptr<threadsafe_queue<std::string>> _fileList;


	friend class State;
	friend class ConnectionClosedState;
	friend class ConnectionReadyState;
	friend class LoginReadyState;
	friend class ReadyForTransferState;
	State* _state;
	std::shared_ptr<Session> _ctrlSession;
	std::shared_ptr<Session> _dataSession;

	std::mutex _m;
	bool _ready_for_transfer;
	std::condition_variable _cv;

};