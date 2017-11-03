#include "state.h"
#include <boost/asio.hpp>
#include <iostream>
#include <regex>
#include <cstdio>
#include "syncqueue.hpp"


using namespace boost;

void State::connect(std::shared_ptr<FtpContext> ftpContext, const std::string& filename, std::function<void()> fun)
{
	ftpContext->GetCtrlSession()->async_connect([this, filename, ftpContext, fun](std::size_t bytes_transferred)
	{
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename, fun](std::size_t bytes_transferred)
		{
			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
			std::string res;
			while (std::getline(is, res) && res[3] == '-');
#ifdef _DEBUG
			fprintf(stdout, "%s\n", res.c_str());
#endif
			if (res.find("220") == 0)
			{
				user(ftpContext, filename, fun);
			}
			else if (res.find("421") == 0)
			{
				ctrl_err(ftpContext, filename, fun);
			}
			else
			{
				connect(ftpContext, filename, fun);
			}
		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
}

void State::user(std::shared_ptr<FtpContext> ftpContext, const std::string& filename, std::function<void()> fun)
{
	std::string request;
	request += "USER ";
	request += ftpContext->GetUser();
	request += "\r\n";
	ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this, fun](std::size_t bytes_transferred)
	{
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this, fun](std::size_t bytes_transferred)
		{
			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				fprintf(stdout, "%s\n", res.c_str());
#endif
				if (res.find("331") == 0)
				{
					pass(ftpContext, filename, fun);
				}
				else
				{
					user(ftpContext, filename, fun);
				}
			}
			else
			{
				user(ftpContext, filename, fun);
			}
		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
}

void State::pass(std::shared_ptr<FtpContext> ftpContext, const std::string& filename, std::function<void()> fun)
{
	std::string request;
	request += "PASS ";
	request += ftpContext->GetPWD();
	request += "\r\n";
	ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this, fun](std::size_t bytes_transferred)
	{
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this, fun](std::size_t bytes_transferred)
		{
			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				fprintf(stdout, "%s\n", res.c_str());
#endif
				if (res.find("230") == 0)
				{
					ChangeStatus(ftpContext, &EPSVReadyState::Instance());
					epsv(ftpContext, filename, fun);
				}
				else if (res.find("530") == 0)
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
					ftpContext->GetCtrlSession()->Close();
					ftpContext->GetDataSession()->Close();
				}
				else
				{
					user(ftpContext, filename, fun);
				}
			}
			else
			{
				user(ftpContext, filename, fun);
			}
		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
}

void State::epsv(std::shared_ptr<FtpContext> ftpContext, const std::string& filename, std::function<void()> fun)
{
	if (ftpContext->GetDataSession())
	{
		ftpContext->GetDataSession()->Close();
		ftpContext->GetDataSession().reset();
	}
	std::string request = "EPSV\r\n";
	ftpContext->GetCtrlSession()->async_send(request, [this, ftpContext, filename, fun](std::size_t bytes_transferred)
	{
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename, fun](std::size_t bytes_transferred)
		{
			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				fprintf(stdout, "%s\n", res.c_str());
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
							ftpContext->GetDataSession()->async_connect([ftpContext, filename, this, fun](std::size_t bytes_transferred)
							{
								fun();
							}, [this,ftpContext,filename,fun]
							{
								data_err(ftpContext);
								epsv(ftpContext, filename, fun);
							});
						}
						else
							epsv(ftpContext, filename, fun);
					}
					catch (const std::invalid_argument& e)
					{
						epsv(ftpContext, filename, fun);
					}
				}
				else
				{
					epsv(ftpContext, filename, fun);
				}
			}
			else
			{
				epsv(ftpContext, filename, fun);
			}
		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
}

void State::retr(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	std::string cmd = "RETR ";
	cmd += filename;
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, filename, ftpContext](std::size_t bytes_transferred)
	{
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename](std::size_t bytes_transferred)
		{
			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
			std::string res;
			if (std::getline(is, res))
			{
#ifdef _DEBUG
				fprintf(stdout, "%s\n", res.c_str());
#endif
				if (res.find("150") == 0)
				{
					ftpContext->GetDataSession()->async_read([this, filename, ftpContext](std::size_t bytes_transferred)
					{
					}, [ftpContext,filename,this]
					{
						if (ftpContext->GetDataSession()->Err().value() == 2)
						{
							std::istream is(ftpContext->GetDataSession()->RecvBuf().get());
							asio::streambuf::const_buffers_type cbt = ftpContext->GetDataSession()->RecvBuf()->data();
							if (asio::buffers_end(cbt) - asio::buffers_begin(cbt) > 0)
							{
								std::string newFileName = filename;
								newFileName.erase(0, newFileName.find_last_of('/') + 1);
								HANDLE hd = ::CreateFile(newFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_FLAG_OVERLAPPED, NULL);
								if (hd == INVALID_HANDLE_VALUE)
								{
									::fprintf(stdout, "File: %s create failed. Error: %d\n", filename.c_str(), GetLastError());
									return;
								}
								std::shared_ptr<boost::asio::windows::stream_handle> sh = std::make_shared<boost::asio::windows::stream_handle>(
									ftpContext->GetIOS(), hd);
								std::shared_ptr<boost::asio::streambuf> buf(std::move(ftpContext->GetDataSession()->RecvBuf()));
								ftpContext->GetDataSession()->Close();
								ftpContext->GetDataSession().reset();
								boost::asio::async_write(*sh, *buf,
								                         [hd, newFileName, filename, ftpContext, this, sh, buf](
								                         const boost::system::error_code& ec, std::size_t bytes_transferred)mutable
								                         {
									                         boost::system::error_code e;
									                         sh->close(e);
									                         buf.reset();
									                         if (ec)
									                         {
										                         ftpContext->_fileList->PutFront(filename);
										                         std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ec.value(),
										                                      ec.message().c_str());
										                         ::DeleteFile(newFileName.c_str());
										                         ftpContext->_fileList->PutFront(filename);
										                         return;
									                         }
									                         ::fprintf(stdout, "Transfer File: %s completed.\n", newFileName.c_str());
								                         });
								ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename](std::size_t bytes_transferred)
								{
									std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
									std::string res;
									if (std::getline(is, res))
									{
#ifdef _DEBUG
										fprintf(stdout, "%s\n", res.c_str());
#endif
										if (res.find("226") == 0)
										{
											ftpContext->ReadyForTransfer();
											if (!ftpContext->_fileList->Empty())
											{
												std::string fn;
												ftpContext->_fileList->Take(fn);
												epsv(ftpContext, fn, std::bind(&State::retr, this, ftpContext, fn));
											}
											else
											{
												ftpContext->GetCtrlSession()->Close();
												ftpContext->GetDataSession()->Close();
											}
										}
										else
										{
											epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
										}
									}
									else
									{
										epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
									}
								}, [this, ftpContext, filename]
								{
									ctrl_err(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
								});
							}
						}
						else
							ftpContext->GetCtrlSession()->Timeout();
					});
				}
				else if (res.find("550") == 0)
				{
					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, "no file");
					ftpContext->ReadyForTransfer();
					if (!ftpContext->_fileList->Empty())
					{
						std::string fn;
						ftpContext->_fileList->Take(fn);
						epsv(ftpContext, fn, std::bind(&State::retr, this, ftpContext, fn));
					}
				}
				else if (res.find("425") == 0)
				{
					epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
				}
				else
				{
					epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
				}
			}
			else
			{
				epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
			}
		}, [this,ftpContext,filename]
		{
			ctrl_err(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
		});
	}, [this,ftpContext,filename]
	{
		ctrl_err(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
	});
}

void State::stor(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	std::string cmd;
	cmd += "STOR ";
	cmd += filename;
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [ftpContext,filename,this](std::size_t bytes_transferred)
	{
		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext,this,filename](std::size_t bytes_transferred)
		{
			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
			std::string res;
			if (std::getline(is, res))
			{
				if (res.find("150") == 0)
				{
					std::string fn = filename;
					fn.erase(0, fn.find_last_of('/') + 1);
					ftpContext->GetDataSession()->transmit_file(
						fn, [this,ftpContext,filename](const boost::system::error_code& ec,std::size_t bytes_transferred)
						{
							ftpContext->GetDataSession()->Close();
							ftpContext->GetDataSession().reset();
							if (ec)
							{
								ftpContext->GetDataSession()->Timeout();
								return;
							}
							ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename](std::size_t bytes_transferred)
							{
								std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
								std::string res;
								if (std::getline(is, res))
								{
#ifdef _DEBUG
									fprintf(stdout, "%s\n", res.c_str());
#endif
									if (res.find("226") == 0)
									{
										ftpContext->ReadyForTransfer();
										if (!ftpContext->_fileList->Empty())
										{
											std::string fn;
											ftpContext->_fileList->Take(fn);
											epsv(ftpContext, fn, std::bind(&State::stor, this, ftpContext, fn));
										}
										else
										{
											ftpContext->GetCtrlSession()->Close();
											ftpContext->GetDataSession()->Close();
										}
									}
									else
									{
										epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
									}
								}
								else
								{
									epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
								}
							}, [this,ftpContext,filename]
							{
								ctrl_err(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
							});
						});
				}
				else
				{
					epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
				}
			}
			else
			{
				epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
			}
		}, [this,ftpContext,filename]
		{
			ctrl_err(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
		});
	}, [this,ftpContext,filename]
	{
		ctrl_err(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
	});
}

void State::nlst(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
}

void State::ctrl_err(std::shared_ptr<FtpContext> ftpContext, const std::string& filename, std::function<void()> fun)
{
	std::fprintf(stderr, "Rebuilding...\n");
	ftpContext->ReBuild([this,ftpContext,filename,fun]
	{
		std::fprintf(stderr, "Connecting...\n");
		connect(ftpContext, "", fun);
	});
}


void EPSVReadyState::DoRecvFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	auto fun = std::bind(&EPSVReadyState::retr, this, ftpContext, filename);
	epsv(ftpContext, filename, fun);
}

void EPSVReadyState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	auto fun = std::bind(&EPSVReadyState::stor, this, ftpContext, filename);
	epsv(ftpContext, filename, fun);
}

void EPSVReadyState::DoList(std::shared_ptr<FtpContext> ftpContext, const std::string& dir)
{
	auto fun = std::bind(&EPSVReadyState::nlst, this, ftpContext, dir);
	epsv(ftpContext, dir, fun);
}
