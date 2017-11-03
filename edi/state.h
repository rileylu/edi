#pragma once
#include <memory>
#include "ftpcontext.h"


class State
{
public:
	static State& Instance()
	{
		static State state;
		return state;
	}

	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, std::string filename)
	{
		connect(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
	}

	virtual void DoRecvFile(std::shared_ptr<FtpContext> ftpContext, std::string filename)
	{
		connect(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
	}

	virtual void DoList(std::shared_ptr<FtpContext> ftpContext, std::string dir)
	{
		connect(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
	}

protected:
	State() = default;

	void ChangeStatus(std::shared_ptr<FtpContext> ftpContext, State* state)
	{
		ftpContext->ChangeStatus(state);
	}

	void connect(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun);
	void user(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun);
	void pass(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun);
	void epsv(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun);
	void retr(std::shared_ptr<FtpContext> ftpContext, std::string filename);
	void stor(std::shared_ptr<FtpContext> ftpContext, std::string filename);
	void nlst(std::shared_ptr<FtpContext> ftpContext, std::string filename);

	void ctrl_err(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun);

	void data_err(std::shared_ptr<FtpContext> ftpContext)
	{
		std::fprintf(stderr, "ErrorCode: %d Message: %s\n", ftpContext->GetDataSession()->Err().value(),
		             ftpContext->GetDataSession()->Err().message().c_str());
		if (ftpContext->GetDataSession())
		{
			ftpContext->GetDataSession()->Close();
			ftpContext->GetDataSession().reset();
		}
	}
};

class EPSVReadyState : public State
{
public:
	static EPSVReadyState& Instance()
	{
		static EPSVReadyState epsvReadyState;
		return epsvReadyState;
	}

	virtual void DoRecvFile(std::shared_ptr<FtpContext> ftpContext, std::string filename) override;
	virtual void DoSendFile(std::shared_ptr<FtpContext> ftpContext, std::string filename) override;
	virtual void DoList(std::shared_ptr<FtpContext> ftpContext, std::string dir) override;
};
