#pragma once
#include <memory>
#include "ftpcontext.h"
class State {
public:
	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename) {};
	virtual void DoRecvFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename) {};
	virtual void DoList(std::shared_ptr<FtpContext> ftpContext, const std::string& dir) {};
protected:
	State() {}
	void ChangeStatus(std::shared_ptr<FtpContext> ftpContext, State* state) {
		ftpContext->ChangeState(state);
	}
};


class ConnectionClosedState :public State {
public:
	static ConnectionClosedState& Instance() {
		static ConnectionClosedState connectionClosedState;
		return connectionClosedState;
	}
	// Inherited via State
	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename) override;
	virtual void DoList(std::shared_ptr<FtpContext> ftpContext, const std::string& dir) override;
};

class ConnectionReadyState :public State {
public:
	static ConnectionReadyState& Instance() {
		static ConnectionReadyState connectionReadyState;
		return connectionReadyState;
	}
	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename) override;
};

class LoginReadyState :public State {
public:
	static LoginReadyState& Instance() {
		static LoginReadyState loginReadyState;
		return loginReadyState;
	}
	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename) override;
};

class ReadyForTransferState :public State {
public:
	static ReadyForTransferState& Instance() {
		static ReadyForTransferState readyForTransferState;
		return readyForTransferState;
	}
	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename) override;
	virtual void DoList(std::shared_ptr<FtpContext> ftpContext, const std::string& dir) override;
};