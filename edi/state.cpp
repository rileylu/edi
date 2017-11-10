#include "state.h"
#include <boost/asio.hpp>
#include <iostream>
#include <regex>
#include <cstdio>
#include "threadsafe_queue.h"

using namespace boost;
//void State::nlst(std::shared_ptr<FtpContext> ftpContext, std::string dir)
//{
//	std::string cmd = "NLST ";
//	cmd += dir;
//	cmd += "\r\n";
//	ftpContext->GetCtrlSession()->async_send(cmd, [this, dir, ftpContext](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, dir](std::size_t bytes_transferred)
//		{
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			if (std::getline(is, res))
//			{
//#ifdef _DEBUG
//				fprintf(stdout, "%s\n", res.c_str());
//#endif
//				if (res.find("150") == 0)
//				{
//					ftpContext->GetDataSession()->async_read([this, dir, ftpContext](std::size_t bytes_transferred)
//					{
//					}, [ftpContext, dir, this]
//					{
//						if (ftpContext->GetDataSession()->Err().value() == 2)
//						{
//							std::istream is(ftpContext->GetDataSession()->RecvBuf().get());
//							asio::streambuf::const_buffers_type cbt = ftpContext->GetDataSession()->RecvBuf()->data();
//							if (asio::buffers_end(cbt) - asio::buffers_begin(cbt) > 0)
//							{
//								std::string newFileName = "list_1.txt";
//								newFileName.erase(0, newFileName.find_last_of('/') + 1);
//								HANDLE hd = ::CreateFile(newFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_FLAG_OVERLAPPED, NULL);
//								if (hd == INVALID_HANDLE_VALUE)
//								{
//									::fprintf(stdout, "File: %s create failed. Error: %d\n", newFileName.c_str(), GetLastError());
//									return;
//								}
//								std::shared_ptr<boost::asio::windows::stream_handle> sh = std::make_shared<boost::asio::windows::stream_handle>(
//									ftpContext->GetIOS(), hd);
//								std::shared_ptr<boost::asio::streambuf> buf(std::move(ftpContext->GetDataSession()->RecvBuf()));
//								ftpContext->GetDataSession()->Close();
//								ftpContext->GetDataSession().reset();
//								boost::asio::async_write(*sh, *buf,
//									[hd, newFileName, dir, ftpContext, this, sh, buf](
//										const boost::system::error_code& ec, std::size_t bytes_transferred)mutable
//								{
//									boost::system::error_code e;
//									sh->close(e);
//									buf.reset();
//									if (ec)
//									{
//										epsv(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//										return;
//									}
//									::fprintf(stdout, "NLST completed.\n");
//								});
//								ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, dir](std::size_t bytes_transferred)
//								{
//									std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//									std::string res;
//									if (std::getline(is, res))
//									{
//#ifdef _DEBUG
//										fprintf(stdout, "%s\n", res.c_str());
//#endif
//										if (res.find("226") == 0)
//										{
//											ftpContext->Close();
//										}
//										else
//										{
//											epsv(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//										}
//									}
//									else
//									{
//										epsv(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//									}
//								}, [this, ftpContext, dir]
//								{
//									ctrl_err(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//								});
//							}
//						}
//					});
//				}
//				else if (res.find("425") == 0)
//				{
//					epsv(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//				}
//				else
//				{
//					epsv(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//				}
//			}
//			else
//			{
//				epsv(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//			}
//		}, [this, ftpContext, dir]
//		{
//			ctrl_err(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//		});
//	}, [this, ftpContext, dir]
//	{
//		ctrl_err(ftpContext, dir, std::bind(&State::nlst, this, ftpContext, dir));
//	});
//}

void State::Run(std::shared_ptr<FtpContext> ftpContext)
{
	connect(ftpContext);
}


void State::session_err(std::shared_ptr<FtpContext> ftpContext)
{
	if (ftpContext->_fileList->empty() && ftpContext->_current_file.size() == 0)
	{
		ftpContext->Close();
		ftpContext.reset();
	}
	else
		ftpContext->ReBuild(*this);
}

inline void State::connect(std::shared_ptr<FtpContext> ftpContext)
{
	ftpContext->GetCtrlSession()->async_connect([this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			parse_response(ftpContext, "220", std::bind(&State::user, this, ftpContext));
		}, std::bind(&State::session_err, this, ftpContext));

	}, std::bind(&State::session_err, this, ftpContext));

}

inline void State::user(std::shared_ptr<FtpContext> ftpContext)
{
	std::string cmd;
	cmd += "USER ";
	cmd += ftpContext->GetUser();
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			parse_response(ftpContext, "331", std::bind(&State::pass, this, ftpContext));
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

inline void State::pass(std::shared_ptr<FtpContext> ftpContext)
{
	std::string cmd;
	cmd += "PASS ";
	cmd += ftpContext->GetPWD();
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			parse_response(ftpContext, "230", std::bind(&State::cwd, this, ftpContext));
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

inline void State::epsv(std::shared_ptr<FtpContext> ftpContext)
{
	ftpContext->GetCtrlSession()->async_send("EPSV\r\n", [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			auto fun = [this, ftpContext] {
				std::regex port_regex(R"(.*\|{3}([0-9]+)\|{1}.*)");
				std::smatch results;
				unsigned short port = 0;
				if (std::regex_search(ftpContext->_res, results, port_regex))
				{
					port = std::stoul(std::string(results[1].first, results[1].second));
					ftpContext->BuildDataSession(port);
					ftpContext->GetDataSession()->async_connect([this, ftpContext]
					{
						FileOP(ftpContext);
					}, [this, ftpContext] {
						if (ftpContext->GetCtrlSession()->Err())
							return;
						else
							epsv(ftpContext);
					});
				}
				else
					epsv(ftpContext);
			};
			parse_response(ftpContext, "229", fun);
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));

}

void State::retr(std::shared_ptr<FtpContext> ftpContext)
{
	if (ftpContext->_current_file.size() == 0)
	{
		if (!ftpContext->_fileList->try_pop(ftpContext->_current_file))
		{
			logout(ftpContext);
			return;
		}
	}
	std::string cmd = "RETR ";
	cmd += ftpContext->_current_file;
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext]
	{
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext]
		{
			auto data = [this, ftpContext] {
				ftpContext->GetDataSession()->async_read([this, ftpContext]
				{
				}, [ftpContext, this]
				{
					if (ftpContext->GetDataSession()->Err().value() == 2)
					{
						//write to file or doing sth. else
						ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext]
						{
							auto fun = [this, ftpContext] {
								auto buf = ftpContext->GetDataSession()->GetSharedRecvBuf();
								ftpContext->GetDataSession()->Close();
								std::string newFileName = ftpContext->_current_file;
								newFileName.erase(0, newFileName.find_last_of('/') + 1);
								HANDLE hd = ::CreateFile(newFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
								if (hd == INVALID_HANDLE_VALUE)
								{
									epsv(ftpContext);
									::fprintf(stderr, "File: %s create failed. Error: %d\n", newFileName.c_str(), GetLastError());
									return;
								}
								std::shared_ptr<boost::asio::windows::stream_handle> sh = std::make_shared<boost::asio::windows::stream_handle>(
									ftpContext->GetIOS(), hd);
								boost::asio::async_write(*sh, *buf,
									[newFileName, ftpContext, this, sh, buf](
										const boost::system::error_code& ec, std::size_t bytes_transferred)mutable
								{
									boost::system::error_code e;
									sh->close(e);
									sh.reset();
									if (ec)
									{
										std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ec.value(),
											ec.message().c_str());
										::DeleteFile(newFileName.c_str());
										return;
									}
									::fprintf(stdout, "Transfer File: %s completed.\n", newFileName.c_str());
								});
								rnfr(ftpContext);
							};
							parse_response(ftpContext, "226", fun);
						}, std::bind(&State::session_err, this, ftpContext));
					}
					else if (ftpContext->GetCtrlSession()->Err())
						return;
					else
						epsv(ftpContext);
				});
			};
			parse_response(ftpContext, "150", data);
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

void State::stor(std::shared_ptr<FtpContext> ftpContext)
{
	if (ftpContext->_current_file.size() == 0)	{		if (!ftpContext->_fileList->try_pop(ftpContext->_current_file))		{			logout(ftpContext);			return;		}	}
	std::string cmd;	cmd += "STOR ";	cmd += "tmp/" + ftpContext->_current_file;	cmd += "\r\n";	ftpContext->GetCtrlSession()->async_send(cmd, [ftpContext, this]	{		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [ftpContext, this]		{			parse_response(ftpContext, "150", [this, ftpContext] {				if (!ftpContext->GetDataSession()->transmit_file(ftpContext->_current_file,					[this, ftpContext](const boost::system::error_code& ec, std::size_t bytes_transferred)				{					ftpContext->GetDataSession()->Close();					if (ec)					{						return;					}				}))				{					ftpContext->_current_file.clear();					epsv(ftpContext);				}				ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {					parse_response(ftpContext, "226", [this, ftpContext] {						rnfr(ftpContext);					});				}, std::bind(&State::session_err, this, ftpContext));			});		}, std::bind(&State::session_err, this, ftpContext));	}, std::bind(&State::session_err, this, ftpContext));
}

void State::rnfr(std::shared_ptr<FtpContext> ftpContext)
{
	std::string cmd;
	cmd += "RNFR ";
	switch (GetType())
	{
	case Type::NLST:
		logout(ftpContext);
		return;
	case Type::RETR:
		cmd += ftpContext->_current_file;
		break;
	case Type::STOR:
		cmd += "tmp/" + ftpContext->_current_file;
		break;
	}
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			auto fun = [this, ftpContext] {
				rnto(ftpContext);
			};
			parse_response(ftpContext, "350", fun);
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

void State::rnto(std::shared_ptr<FtpContext> ftpContext)
{
	std::string cmd;
	cmd += "RNTO ";
	switch (GetType())
	{
	case Type::RETR:
		cmd += "bak/" + ftpContext->_current_file;
		break;
	case Type::STOR:
		cmd += ftpContext->_current_file;
		break;
	}
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			auto fun = [this, ftpContext] {
				if (ftpContext->_fileList->try_pop(ftpContext->_current_file))
					epsv(ftpContext);
				else
					logout(ftpContext);
			};
			parse_response(ftpContext, "250", fun);
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));

}

void State::nlst(std::shared_ptr<FtpContext> ftpContext)
{
}

void State::logout(std::shared_ptr<FtpContext> ftpContext)
{
	ftpContext->GetCtrlSession()->async_send("QUIT\r\n", [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			parse_response(ftpContext, "221", [this, ftpContext]()mutable {
				ftpContext->Close();
				ftpContext.reset();
			});
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));

}

void State::cwd(std::shared_ptr<FtpContext> ftpContext)
{
	std::string cmd;
	cmd += "CWD ";
	cmd += ftpContext->_dir;
	cmd += "\r\n";
	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			parse_response(ftpContext, "250", std::bind(&State::epsv, this, ftpContext));
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

RetrState & RetrState::Instance()
{
	static RetrState recvState;
	return recvState;
}

void RetrState::FileOP(std::shared_ptr<FtpContext> ftpContext)
{
	retr(ftpContext);
}

State::Type RetrState::GetType() const
{
	return State::Type::RETR;
}

StorState & StorState::Instance()
{
	static StorState storState;
	return storState;
}

State::Type StorState::GetType() const
{
	return Type::STOR;
}

void StorState::FileOP(std::shared_ptr<FtpContext> ftpContext)
{
	stor(ftpContext);
}

NlstState & NlstState::Instance()
{
	static NlstState nlstState;
	return nlstState;
}

State::Type NlstState::GetType() const
{
	return State::Type::NLST;
}

void NlstState::FileOP(std::shared_ptr<FtpContext> ftpContext)
{
	nlst(ftpContext);
}
