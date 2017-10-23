#include "state.h"
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>
#include "threadsafe_queue.h"


using namespace boost;
#define FTPERROR "FTPERROR"

void ConnectionClosedState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	try
	{
		ftpContext->GetCtrlSession()->async_connect([ftpContext, filename, this] {
			ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this] {
				std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
				std::string res;
				while (std::getline(is, res))
				{
					if (res[3] == '-')
						continue;
					else if (res.find("220") == 0)
					{
						ChangeStatus(ftpContext, &ConnectionReadyState::Instance());
						ftpContext->DoSendFile(filename);
					}
					else
						throw std::exception(FTPERROR);
				}
			});
		});
	}
	catch (const std::exception &ec)
	{
		std::cerr << ec.what() << std::endl;
		ftpContext.reset(new FtpContext(ftpContext->GetIOS(), ftpContext->GetIP(), ftpContext->GetPort(), ftpContext->GetUser(), ftpContext->GetPWD()));
		ftpContext->DoSendFile(filename);
	}
}

void ConnectionClosedState::DoList(std::shared_ptr<FtpContext> ftpContext, const std::string & dir)
{
}

void ConnectionReadyState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string & filename)
{
	std::string request;
	request += "USER ";
	request += ftpContext->GetUser();
	request += "\r\n";
	try
	{
		ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this] {
			ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this] {
				std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
				std::string res;
				if (std::getline(is, res))
				{
					if (res.find("331") == 0)
					{
						std::string request;
						request += "PASS ";
						request += ftpContext->GetPWD();
						request += "\r\n";
						ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this] {
							ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this] {
								std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
								std::string res;
								if (std::getline(is, res))
								{
									if (res.find(230) == 0)
									{
										ChangeStatus(ftpContext, &LoginReadyState::Instance());
										ftpContext->DoSendFile(filename);
									}
									else
										throw std::exception(FTPERROR);
								}
							});
						});
					}
					else
						throw std::exception(FTPERROR);
				}
				else
					throw std::exception(FTPERROR);
			});
		});
	}
	catch (const std::exception& ec)
	{
		std::cerr << ec.what() << std::endl;
		ftpContext.reset(new FtpContext(ftpContext->GetIOS(), ftpContext->GetIP(), ftpContext->GetPort(), ftpContext->GetUser(), ftpContext->GetPWD()));
		ftpContext->DoSendFile(filename);
	}
}

void LoginReadyState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string & filename)
{
	std::string request = "EPSV\r\n";
	try
	{
		ftpContext->GetCtrlSession()->async_send(request, [this, ftpContext, filename] {
			ftpContext->GetCtrlSession()->async_read([this, ftpContext, filename] {
				std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
				std::string res;
				if (res.find("229") == 0)
				{
					try
					{
						std::regex port_regex(R"(.*\|{3}([0-9]+)\|{1}.*)");
						std::smatch results;
						unsigned short port = 0;
						if (std::regex_search(res, results, port_regex))
						{
							port = std::stoul(std::string(results[1].first, results[1].second));
							ftpContext->BuildDataSession(port);
							try {
								ftpContext->GetDataSession()->async_connect([ftpContext, filename, this] {
									ChangeStatus(ftpContext, &ReadyForTransferState::Instance());
									ftpContext->DoSendFile(filename);
								});
							}
							catch (const system::error_code& ec)
							{
								std::cout << ec.message() << std::endl;
								ftpContext->DoSendFile(filename);
							}
						}
						else
							ftpContext->DoSendFile(filename);
					}
					catch (const std::invalid_argument &e)
					{
						ftpContext->DoSendFile(filename);
					}
				}
				else
					throw std::exception(FTPERROR);

			});
		});
	}
	catch (const std::exception& ec)
	{
		std::cerr << ec.what() << std::endl;
		ftpContext.reset(new FtpContext(ftpContext->GetIOS(), ftpContext->GetIP(), ftpContext->GetPort(), ftpContext->GetUser(), ftpContext->GetPWD()));
		ftpContext->DoSendFile(filename);
	}
}

void ReadyForTransferState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string & filename)
{
	std::string cmd = "RETR ";
	cmd += filename;
	cmd += "\r\n";
	try
	{
		ftpContext->GetCtrlSession()->async_send(cmd, [this, filename, ftpContext] {
			ftpContext->GetCtrlSession()->async_read([this, filename, ftpContext] {
				if (ftpContext->GetDataSession()->Err().value() == 2)
				{
					std::istream is(ftpContext->GetDataSession()->ResponseBuf());
					asio::streambuf::const_buffers_type cbt = ftpContext->GetDataSession()->ResponseBuf()->data();
					std::string newFileName = filename;
					std::ofstream of(newFileName);
					std::copy(asio::buffers_begin(cbt), asio::buffers_end(cbt), std::ostream_iterator<char>(of));
					of.flush();
					of.close();
					ftpContext->GetDataSession()->Close();
				}
				ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename] {
					std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
					std::string res;
					if (std::getline(is, res))
					{
						if (res.find("150") == 0)
						{
							ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename] {
								std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
								std::string res;
								if (std::getline(is, res))
								{
									if (res.find("226") == 0)
									{
										ChangeStatus(ftpContext, &LoginReadyState::Instance());
										ftpContext->ReadyForTransfer();
										if (!ftpContext->_fileList->empty())
										{
											std::string fn;
											ftpContext->_fileList->wait_and_pop(fn);
											ftpContext->DoSendFile(fn);
										}
									}
									else
									{
										throw std::exception(FTPERROR);
									}
								}
							});
						}
						else
						{
							throw std::exception(FTPERROR);
						}
					}
					else
					{
						throw std::exception(FTPERROR);
					}
				});
			});
		});
	}
	catch (const std::exception& ec)
	{
		std::cerr << ec.what() << std::endl;
		ftpContext.reset(new FtpContext(ftpContext->GetIOS(), ftpContext->GetIP(), ftpContext->GetPort(), ftpContext->GetUser(), ftpContext->GetPWD()));
		ftpContext->DoSendFile(filename);
	}
}

void ReadyForTransferState::DoList(std::shared_ptr<FtpContext> ftpContext, const std::string & dir)
{
}
