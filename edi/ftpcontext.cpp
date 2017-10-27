#include "ftpcontext.h"
#include <boost/asio.hpp>
#include "state.h"
#include "syncqueue.hpp"

using namespace boost;
FtpContext::FtpContext(asio::io_service& ios, const std::string & ip, unsigned short port, const std::string& user, const std::string& pwd)
	:_ios(ios), _ip_address(ip), _port(port), _user(user), _pwd(pwd), _fileList(new SyncQueue<std::string>), _state(&ConnectionClosedState::Instance()), _ctrlSession(nullptr), _dataSession(nullptr), _ready_for_transfer(true)
{
	_ctrlSession = std::make_shared<Session>(ios, ip, port);
}

void FtpContext::SendFile(const std::string & fileName)
{
	WaitForTransfer();
	DoSendFile(fileName);
}

void FtpContext::SendFile(std::shared_ptr<SyncQueue<std::string>> fileList)
{
	_fileList = fileList;
	std::string filename;
	_fileList->Take(filename);
	DoSendFile(filename);
}

void FtpContext::RecvFile(const std::string & fileName)
{
}

void FtpContext::List(const std::string & dir)
{
}
void FtpContext::DoSendFile(const std::string& filename) {
	_state->DoSendFile(shared_from_this(), filename);
}
void FtpContext::ReBuild() {
	//auto old = _ctrlSession;
	_ctrlSession.reset(new Session(_ios, _ip_address, _port));
	//if(old)
	//	old->Close();
	//old = _dataSession;
	_dataSession.reset();
	//if(old)
	//	old->Close();
	ChangeState(&ConnectionClosedState::Instance());
}
