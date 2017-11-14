#include "ftpcontext.h"
#include <boost/asio.hpp>
#include "state.h"
#include "syncqueue.hpp"

using namespace boost;

FtpContext::FtpContext(asio::io_service& ios, const std::string& ip, unsigned short port, const std::string& user,
	const std::string& pwd, const std::string& dir, std::shared_ptr<threadsafe_queue<std::string>> fileList)
	: _ios(ios), _ip_address(ip), _port(port), _user(user), _pwd(pwd), _dir(dir), _fileList(fileList), _ctrlSession(nullptr), _dataSession(nullptr)
{
	_ctrlSession = std::move(std::make_unique<FtpSession>(ios, ip, port));
}

void FtpContext::SendFile()
{
	StorState::Instance().Run(shared_from_this());
}

void FtpContext::RecvFile()
{
	RetrState::Instance().Run(shared_from_this());
}

void FtpContext::List()
{
	NlstState::Instance().Run(shared_from_this());
}

void FtpContext::ReBuild(State & s)
{
	_ctrlSession->Timer().cancel(_ctrlSession->Err());
	_ctrlSession.reset(new FtpSession(_ios, _ip_address, _port));
	if (_dataSession)
	{
		_dataSession->Close();
		_dataSession.reset(nullptr);
	}
	_ctrlSession->Timer().expires_from_now(std::chrono::seconds(30));
	_ctrlSession->Timer().async_wait(std::bind(&State::connect, &s, shared_from_this()));
}
