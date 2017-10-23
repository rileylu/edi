#include "ftpcontext.h"
#include <boost/asio.hpp>
#include "state.h"
#include "threadsafe_queue.h"


FtpContext::FtpContext(asio::io_service& ios, const std::string & ip, unsigned short port, const std::string& user, const std::string& pwd)
	:_ios(ios), _ip_address(ip), _user(user), _pwd(pwd), _state(&ConnectionClosedState::Instance()), _ctrlSession(nullptr), _dataSession(nullptr), _ready_for_transfer(true), _data_connection_finished(true)
{
	_ctrlSession = std::make_shared<Session>(ios, ip, port);
}

void FtpContext::SendFile(const std::string & fileName)
{
	WaitForTransfer();
	DoSendFile(fileName);
}

void FtpContext::SendFile(std::shared_ptr<boost::lockfree::queue<std::string*>> fileList)
{
	_fileList = fileList;
	std::string *filename = nullptr;
	if (fileList->pop(filename))
	{
		DoSendFile(*filename);
		delete filename;
	}
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
