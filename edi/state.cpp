#include "state.h"
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>
#include <cstdio>
#include "syncqueue.hpp"


using namespace boost;
#define FTPERROR "FTPERROR"

void ConnectionClosedState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	ftpContext->GetCtrlSession()->async_connect([this, ftpContext, filename] {
		if (ftpContext->GetCtrlSession()->Err())
		{
			std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
			ftpContext->ReBuild();
			ftpContext->DoSendFile(filename);
			return;
		}
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename] {
			if (ftpContext->GetCtrlSession()->Err())
			{
				std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
				ftpContext->ReBuild();
				ftpContext->DoSendFile(filename);
				return;
			}
			std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
			std::string res;
			while (std::getline(is, res))
			{
#ifdef _DEBUG
				std::cout << res << std::endl;
#endif
				if (res[3] == '-')
					continue;
				else if (res.find("220") == 0)
				{
					ChangeStatus(ftpContext, &ConnectionReadyState::Instance());
					ftpContext->DoSendFile(filename);
				}
				else if (res.find("421") == 0)
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
					ftpContext->_fileList->Put(filename);
					ftpContext->GetCtrlSession().reset();
					ftpContext->GetDataSession().reset();
				}
				else
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
					ftpContext->ReBuild();
					ftpContext->DoSendFile(filename);
				}
			}
		});
	});
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
	ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this] {
		if (ftpContext->GetCtrlSession()->Err())
		{
			std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
			ftpContext->ReBuild();
			ftpContext->DoSendFile(filename);
			return;
		}
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this] {
			if (ftpContext->GetCtrlSession()->Err())
			{
				std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
				ftpContext->ReBuild();
				ftpContext->DoSendFile(filename);
				return;
			}
			std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				std::cout << res << std::endl;
#endif
				if (res.find("331") == 0)
				{
					std::string request;
					request += "PASS ";
					request += ftpContext->GetPWD();
					request += "\r\n";
					ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this] {
						if (ftpContext->GetCtrlSession()->Err())
						{
							std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
							ftpContext->ReBuild();
							ftpContext->DoSendFile(filename);
							return;
						}
						ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this] {
							if (ftpContext->GetCtrlSession()->Err())
							{
								std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
								ftpContext->ReBuild();
								ftpContext->DoSendFile(filename);
								return;
							}
							std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
							std::string res;
							if (std::getline(is, res))
							{
#ifdef _DEBUG
								std::cout << res << std::endl;
#endif
								if (res.find("230") == 0)
								{
									ChangeStatus(ftpContext, &LoginReadyState::Instance());
									ftpContext->DoSendFile(filename);
								}
								else
								{
									std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
									ftpContext->ReBuild();
									ftpContext->DoSendFile(filename);
									return;
								}
							}
							else
							{
								ftpContext->ReBuild();
								ftpContext->DoSendFile(filename);
								return;
							}
						});
					});
				}
				else
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
					ftpContext->ReBuild();
					ftpContext->DoSendFile(filename);
					return;
				}
			}
			else
			{
				ftpContext->ReBuild();
				ftpContext->DoSendFile(filename);
				return;
			}
		});
	});
}

void LoginReadyState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string & filename)
{
	std::string request = "EPSV\r\n";
	ftpContext->GetCtrlSession()->async_send(request, [this, ftpContext, filename] {
		if (ftpContext->GetCtrlSession()->Err())
		{
			std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
			ftpContext->ReBuild();
			ftpContext->DoSendFile(filename);
			return;
		}
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename] {
			if (ftpContext->GetCtrlSession()->Err())
			{
				std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
				ftpContext->DoSendFile(filename);
				return;
			}
			std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				std::cout << res << std::endl;
#endif
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
							ftpContext->GetDataSession()->async_connect([ftpContext, filename, this] {
								if (ftpContext->GetDataSession()->Err())
								{
									std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetDataSession()->Err().value(), ftpContext->GetDataSession()->Err().message().c_str());
									ftpContext->DoSendFile(filename);
									return;
								}
								ChangeStatus(ftpContext, &ReadyForTransferState::Instance());
								ftpContext->DoSendFile(filename);
							});
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
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
					ftpContext->ReBuild();
					ftpContext->DoSendFile(filename);
					return;
				}

			}
			else
			{
				ftpContext->ReBuild();
				ftpContext->DoSendFile(filename);
				return;
			}
		});
	});
}

void ReadyForTransferState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string & filename)
{
	std::string cmd = "RETR ";
	cmd += filename;
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, filename, ftpContext] {
		if (ftpContext->GetCtrlSession()->Err())
		{
			std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
			ftpContext->ReBuild();
			ftpContext->DoSendFile(filename);
			return;
		}
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename] {
			if (ftpContext->GetCtrlSession()->Err())
			{
				std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
				ftpContext->ReBuild();
				ftpContext->DoSendFile(filename);
				return;
			}
			std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				std::cout << res << std::endl;
#endif
				if (res.find("150") == 0)
				{
					ftpContext->GetDataSession()->async_read([this, filename, ftpContext] {
						if (ftpContext->GetDataSession()->Err().value() == 2)
						{
							std::istream is(ftpContext->GetDataSession()->ResponseBuf());
							asio::streambuf::const_buffers_type cbt = ftpContext->GetDataSession()->ResponseBuf()->data();
							if (asio::buffers_end(cbt) - asio::buffers_begin(cbt) > 0)
							{
								std::string newFileName = filename;
								newFileName.erase(0, newFileName.find_last_of('/') + 1);
								std::ofstream of(newFileName);
								std::copy(asio::buffers_begin(cbt), asio::buffers_end(cbt), std::ostream_iterator<char>(of));
								of.flush();
								of.close();
								::fprintf(stdout, "Transfer File: %s completed.\n", filename.c_str());
							}
						}
						else if (ftpContext->GetDataSession()->Err())
						{
							std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetDataSession()->Err().value(), ftpContext->GetDataSession()->Err().message().c_str());
							ChangeStatus(ftpContext, &LoginReadyState::Instance());
							ftpContext->DoSendFile(filename);
							return;
						}
						ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename] {
							if (ftpContext->GetCtrlSession()->Err())
							{
								std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ftpContext->GetCtrlSession()->Err().value(), ftpContext->GetCtrlSession()->Err().message().c_str());
								ftpContext->ReBuild();
								ftpContext->DoSendFile(filename);
								return;
							}
							std::istream is(ftpContext->GetCtrlSession()->ResponseBuf());
							std::string res;
							if (std::getline(is, res))
							{
#ifdef _DEBUG
								std::cout << res << std::endl;
#endif
								if (res.find("226") == 0)
								{
									ChangeStatus(ftpContext, &LoginReadyState::Instance());
									ftpContext->ReadyForTransfer();
									if (!ftpContext->_fileList->Empty())
									{
										std::string fn;
										ftpContext->_fileList->Take(fn);
										ftpContext->DoSendFile(fn);
									}
								}
								else
								{
									std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
									ftpContext->ReBuild();
									ftpContext->DoSendFile(filename);
									return;
								}
							}
						});
					});
				}
				else if (res.find("550") == 0)
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, "no file");
					ChangeStatus(ftpContext, &LoginReadyState::Instance());
					ftpContext->ReadyForTransfer();
					if (!ftpContext->_fileList->Empty())
					{
						std::string fn;
						ftpContext->_fileList->Take(fn);
						ftpContext->DoSendFile(fn);
					}
				}
				else
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
					ftpContext->ReBuild();
					ftpContext->DoSendFile(filename);
					return;
				}
			}
			else
			{
				std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
				ftpContext->ReBuild();
				ftpContext->DoSendFile(filename);
				return;
			}
		});
	});
}

void ReadyForTransferState::DoList(std::shared_ptr<FtpContext> ftpContext, const std::string & dir)
{
}
