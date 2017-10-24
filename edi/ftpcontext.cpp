#include "ftpcontext.h"
#include <boost/asio.hpp>
#include "state.h"
#include "threadsafe_queue.h"

using namespace boost;
FtpContext::FtpContext(asio::io_service& ios, const std::string & ip, unsigned short port, const std::string& user, const std::string& pwd)
	:_ios(ios), _ip_address(ip), _port(port), _user(user), _pwd(pwd), _fileList(std::make_shared<threadsafe_queue<std::string>>()), _state(&ConnectionClosedState::Instance()), _ctrlSession(nullptr), _dataSession(nullptr), _ready_for_transfer(true)
{
	_ctrlSession = std::make_shared<Session>(ios, ip, port);
}

void FtpContext::SendFile(const std::string & fileName)
{
	WaitForTransfer();
	DoSendFile(fileName);
}

void FtpContext::SendFile(std::shared_ptr<threadsafe_queue<std::string>> fileList)
{
	_fileList = fileList;
	std::string filename;
	_fileList->wait_and_pop(filename);
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
	_ctrlSession.reset(new Session(_ios, _ip_address, _port));
	_dataSession.reset();
	ChangeState(&ConnectionClosedState::Instance());
}
