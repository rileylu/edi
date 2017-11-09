#include "state.h"
#include <boost/asio.hpp>
#include <iostream>
#include <regex>
#include <cstdio>
#include "syncqueue.hpp"

using namespace boost;
//
//void State::connect(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun)
//{
//	ftpContext->GetCtrlSession()->async_connect([this, filename, ftpContext, fun](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename, fun](std::size_t bytes_transferred)
//		{
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			while (std::getline(is, res) && res[3] == '-');
//#ifdef _DEBUG
//			fprintf(stdout, "%s\n", res.c_str());
//#endif
//			if (res.find("220") == 0)
//			{
//				user(ftpContext, filename, fun);
//			}
//			else if (res.find("421") == 0)
//			{
//				ctrl_err(ftpContext, filename, fun);
//			}
//			else
//			{
//				connect(ftpContext, filename, fun);
//			}
//		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//}
//
//void State::user(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun)
//{
//	std::string request;
//	request += "USER ";
//	request += ftpContext->GetUser();
//	request += "\r\n";
//	ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this, fun](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this, fun](std::size_t bytes_transferred)
//		{
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			if (std::getline(is, res))
//			{
//#ifdef _DEBUG
//				fprintf(stdout, "%s\n", res.c_str());
//#endif
//				if (res.find("331") == 0)
//				{
//					pass(ftpContext, filename, fun);
//				}
//				else
//				{
//					user(ftpContext, filename, fun);
//				}
//			}
//			else
//			{
//				user(ftpContext, filename, fun);
//			}
//		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//}
//
//void State::pass(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun)
//{
//	std::string request;
//	request += "PASS ";
//	request += ftpContext->GetPWD();
//	request += "\r\n";
//	ftpContext->GetCtrlSession()->async_send(request, [ftpContext, filename, this, fun](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, filename, this, fun](std::size_t bytes_transferred)
//		{
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			if (std::getline(is, res))
//			{
//#ifdef _DEBUG
//				fprintf(stdout, "%s\n", res.c_str());
//#endif
//				if (res.find("230") == 0)
//				{
//					ChangeStatus(ftpContext, &EPSVReadyState::Instance());
//					epsv(ftpContext, filename, fun);
//				}
//				else if (res.find("530") == 0)
//				{
//					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, res.c_str());
//					ftpContext->GetCtrlSession()->Close();
//					ftpContext->GetDataSession()->Close();
//				}
//				else
//				{
//					user(ftpContext, filename, fun);
//				}
//			}
//			else
//			{
//				user(ftpContext, filename, fun);
//			}
//		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//}
//
//void State::epsv(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun)
//{
//	std::string request = "EPSV\r\n";
//	ftpContext->GetCtrlSession()->async_send(request, [this, ftpContext, filename, fun](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename, fun](std::size_t bytes_transferred)
//		{
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			if (std::getline(is, res))
//			{
//#ifdef _DEBUG
//				fprintf(stdout, "%s\n", res.c_str());
//#endif
//				if (res.find("229") == 0)
//				{
//					try
//					{
//						std::regex port_regex(R"(.*\|{3}([0-9]+)\|{1}.*)");
//						std::smatch results;
//						unsigned short port = 0;
//						if (std::regex_search(res, results, port_regex))
//						{
//							port = std::stoul(std::string(results[1].first, results[1].second));
//							ftpContext->BuildDataSession(port);
//							ftpContext->GetDataSession()->async_connect([ftpContext, filename, this, fun](std::size_t bytes_transferred)
//							{
//								fun();
//							}, [this, ftpContext, filename, fun]
//							{
//								data_err(ftpContext);
//								epsv(ftpContext, filename, fun);
//							});
//						}
//						else
//							epsv(ftpContext, filename, fun);
//					}
//					catch (const std::invalid_argument& e)
//					{
//						epsv(ftpContext, filename, fun);
//					}
//				}
//				else
//				{
//					epsv(ftpContext, filename, fun);
//				}
//			}
//			else
//			{
//				epsv(ftpContext, filename, fun);
//			}
//		}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//	}, std::bind(&State::ctrl_err, this, ftpContext, filename, fun));
//}
//
//void State::retr(std::shared_ptr<FtpContext> ftpContext, std::string filename)
//{
//	ftpContext->GetDataSession()->async_read([this, filename, ftpContext](std::size_t bytes_transferred)
//	{
//	}, [ftpContext, filename, this]
//	{
//		if (ftpContext->GetDataSession()->Err().value() == 2)
//		{
//			std::string newFileName = filename;
//			newFileName.erase(0, newFileName.find_last_of('/') + 1);
//			HANDLE hd = ::CreateFile(newFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_FLAG_OVERLAPPED, NULL);
//			if (hd == INVALID_HANDLE_VALUE)
//			{
//				::fprintf(stdout, "File: %s create failed. Error: %d\n", filename.c_str(), GetLastError());
//				return;
//			}
//			std::shared_ptr<boost::asio::windows::stream_handle> sh = std::make_shared<boost::asio::windows::stream_handle>(
//				ftpContext->GetIOS(), hd);
//			boost::asio::async_write(*sh, *ftpContext->GetDataSession()->RecvBuf(),
//				[newFileName, filename, ftpContext, this, sh](
//					const boost::system::error_code& ec, std::size_t bytes_transferred)mutable
//			{
//				boost::system::error_code e;
//				sh->close(e);
//				sh.reset();
//				ftpContext->GetDataSession()->Close();
//				ftpContext->GetDataSession().reset();
//				if (ec)
//				{
//					ftpContext->_fileList->PutFront(filename);
//					std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ec.value(),
//						ec.message().c_str());
//					::DeleteFile(newFileName.c_str());
//					ftpContext->_fileList->PutFront(filename);
//					return;
//				}
//				::fprintf(stdout, "Transfer File: %s completed.\n", newFileName.c_str());
//			});
//		}
//	});
//	std::string cmd = "RETR ";
//	cmd += filename;
//	cmd += "\r\n";
//	ftpContext->GetCtrlSession()->async_send(cmd, [this, filename, ftpContext](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename](std::size_t bytes_transferred)
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
//					ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename](std::size_t bytes_transferred)
//					{
//						std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//						std::string res;
//						if (std::getline(is, res))
//						{
//#ifdef _DEBUG
//							fprintf(stdout, "%s\n", res.c_str());
//#endif
//							if (res.find("226") == 0)
//							{
//								ftpContext->ReadyForTransfer();
//								if (!ftpContext->_fileList->Empty())
//								{
//									std::string fn;
//									ftpContext->_fileList->Take(fn);
//									epsv(ftpContext, fn, std::bind(&State::retr, this, ftpContext, fn));
//								}
//								else
//								{
//									ftpContext->Close();
//								}
//							}
//							else
//							{
//								epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//							}
//						}
//						else
//						{
//							epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//						}
//					}, [this, ftpContext, filename]
//					{
//						ctrl_err(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//					});
//				}
//				else if (res.find("550") == 0)
//				{
//					std::fprintf(stderr, "Line: %d Message: %s\n", __LINE__, "no file");
//					ftpContext->ReadyForTransfer();
//					if (!ftpContext->_fileList->Empty())
//					{
//						std::string fn;
//						ftpContext->_fileList->Take(fn);
//						epsv(ftpContext, fn, std::bind(&State::retr, this, ftpContext, fn));
//					}
//				}
//				else if (res.find("425") == 0)
//				{
//					epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//				}
//				else
//				{
//					epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//				}
//			}
//			else
//			{
//				epsv(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//			}
//		}, [this, ftpContext, filename]
//		{
//			ctrl_err(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//		});
//	}, [this, ftpContext, filename]
//	{
//		ctrl_err(ftpContext, filename, std::bind(&State::retr, this, ftpContext, filename));
//	});
//}
//
//void State::stor(std::shared_ptr<FtpContext> ftpContext, std::string filename)
//{
//	std::string fn = filename;
//	fn.erase(0, fn.find_last_of('/') + 1);
//	if (!ftpContext->GetDataSession()->transmit_file(
//		fn, [this, ftpContext, filename](const boost::system::error_code& ec, std::size_t bytes_transferred)
//	{
//		ftpContext->GetDataSession()->Close();
//		ftpContext->GetDataSession().reset();
//		if (ec)
//		{
//			//ftpContext->GetCtrlSession()->Timeout();
//			return;
//		}
//	}))
//	{
//		if (!ftpContext->_fileList->Empty())
//		{
//			std::string fn;
//			ftpContext->_fileList->Take(fn);
//			epsv(ftpContext, fn, std::bind(&State::stor, this, ftpContext, fn));
//		}
//		else
//			ftpContext->Close();
//		return;
//	}
//	std::string cmd;
//	cmd += "STOR ";
//	cmd += filename;
//	cmd += "\r\n";
//	ftpContext->GetCtrlSession()->async_send(cmd, [ftpContext, filename, this](std::size_t bytes_transferred)
//	{
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [ftpContext, this, filename](std::size_t bytes_transferred)
//		{
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			if (std::getline(is, res))
//			{
//				if (res.find("150") == 0)
//				{
//					ftpContext->GetCtrlSession()->async_readutil("\r\n", [this, ftpContext, filename](std::size_t bytes_transferred)
//					{
//						std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//						std::string res;
//						if (std::getline(is, res))
//						{
//#ifdef _DEBUG
//							fprintf(stdout, "%s\n", res.c_str());
//#endif
//							if (res.find("226") == 0)
//							{
//								ftpContext->ReadyForTransfer();
//								if (!ftpContext->_fileList->Empty())
//								{
//									std::string fn;
//									ftpContext->_fileList->Take(fn);
//									epsv(ftpContext, fn, std::bind(&State::stor, this, ftpContext, fn));
//								}
//								else
//								{
//									ftpContext->Close();
//								}
//							}
//							else
//							{
//								epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//							}
//						}
//						else
//						{
//							epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//						}
//					}, [this, ftpContext, filename]
//					{
//						ctrl_err(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//					});
//				}
//				else
//				{
//					epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//				}
//			}
//			else
//			{
//				epsv(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//			}
//		}, [this, ftpContext, filename]
//		{
//			ctrl_err(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//		});
//	}, [this, ftpContext, filename]
//	{
//		ctrl_err(ftpContext, filename, std::bind(&State::stor, this, ftpContext, filename));
//	});
//}
//
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
//
//void State::ctrl_err(std::shared_ptr<FtpContext> ftpContext, std::string filename, std::function<void()> fun)
//{
//	std::fprintf(stderr, "Rebuild in 30 secs...\n");
//	ftpContext->_fileList->PutFront(filename);
//	ftpContext->ReBuild([this, ftpContext, fun]
//	{
//		if (!ftpContext->_fileList->Empty())
//		{
//			std::string fn;
//			std::fprintf(stderr, "Connecting...\n");
//			ftpContext->_fileList->Take(fn);
//			connect(ftpContext, fn, fun);
//		}
//		else
//			ftpContext->Close();
//	});
//}
//
//void EPSVReadyState::DoRecvFile(std::shared_ptr<FtpContext> ftpContext, std::string filename)
//{
//	auto fun = std::bind(&EPSVReadyState::retr, this, ftpContext, filename);
//	epsv(ftpContext, filename, fun);
//}
//
//void EPSVReadyState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, std::string filename)
//{
//	auto fun = std::bind(&EPSVReadyState::stor, this, ftpContext, filename);
//	epsv(ftpContext, filename, fun);
//}
//
//void EPSVReadyState::DoList(std::shared_ptr<FtpContext> ftpContext, std::string dir)
//{
//	auto fun = std::bind(&EPSVReadyState::nlst, this, ftpContext, dir);
//	epsv(ftpContext, dir, fun);
//}
//
//void InitialState::DoSendFile(std::shared_ptr<FtpContext> ftpContext)
//{
//}
//
//void InitialState::DoRecvFile(std::shared_ptr<FtpContext> ftpContext)
//{
//}
//
//void InitialState::DoList(std::shared_ptr<FtpContext> ftpContext)
//{
//}
//
//void State::connect(std::shared_ptr<FtpSession> session, std::shared_ptr<FtpContext> ftpContext, State * nextState)
//{
//}
//
//void State::SendCMD(const std::string & cmd, std::shared_ptr<FtpContext> ftpContext, State * nextState)
//{
//	ftpContext->GetCtrlSession()->async_send(cmd, [this, ftpContext, nextState] {
//		ChangeStatus(ftpContext, nextState);
//	}, [] {
//	})
//}
//
//void State::connect(std::shared_ptr<FtpContext> ftpContext)
//{
//	ftpContext->GetCtrlSession()->async_connect([] {
//		ftpContext->GetCtrlSession()->async_readutil("\r\n", [] {
//			std::istream is(ftpContext->GetCtrlSession()->RecvBuf().get());
//			std::string res;
//			while (std::getline(is, res) && res[3] == '-');
//			std::getline(is, res);
//			if (res.find("220") == 0)
//			{
//			}
//			else
//			{
//			}
//		}, [] {})
//	}, [] {});
//}

void State::Run(std::shared_ptr<FtpContext> ftpContext)
{
	connect(ftpContext);
}


void State::session_err(std::shared_ptr<FtpContext> ftpContext)
{
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
			parse_response(ftpContext, "230", std::bind(&State::epsv, this, ftpContext));
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

inline void State::epsv(std::shared_ptr<FtpContext> ftpContext)
{
	ftpContext->GetCtrlSession()->async_send("EPSV\r\n", [this, ftpContext] {
		ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext] {
			auto fun = [this, ftpContext] {
				try
				{
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
						}, std::bind(&State::epsv, this, ftpContext));
					}
					else
						epsv(ftpContext);
				}
				catch (const std::invalid_argument& e)
				{
					epsv(ftpContext);
				}
			};
			parse_response(ftpContext, "229", fun);
		}, std::bind(&State::session_err, this, ftpContext));
	}, std::bind(&State::session_err, this, ftpContext));
}

void State::retr(std::shared_ptr<FtpContext> ftpContext)
{
	if (ftpContext->_current_file.size() == 0)
	{
		if (!ftpContext->_fileList->Take(ftpContext->_current_file))
		{
			ftpContext->Close();
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
						std::string newFileName = ftpContext->_current_file;
						newFileName.erase(0, newFileName.find_last_of('/') + 1);
						HANDLE hd = ::CreateFile(newFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_FLAG_OVERLAPPED, NULL);
						if (hd == INVALID_HANDLE_VALUE)
						{
							::fprintf(stdout, "File: %s create failed. Error: %d\n", newFileName.c_str(), GetLastError());
							;
							return;
						}
						std::shared_ptr<boost::asio::windows::stream_handle> sh = std::make_shared<boost::asio::windows::stream_handle>(
							ftpContext->GetIOS(), hd);
						boost::asio::async_write(*sh, *ftpContext->GetDataSession()->RecvBuf(),
							[newFileName, ftpContext, this, sh](
								const boost::system::error_code& ec, std::size_t bytes_transferred)mutable
						{
							boost::system::error_code e;
							sh->close(e);
							sh.reset();
							ftpContext->GetDataSession()->Close();
							ftpContext->GetDataSession().reset();
							if (ec)
							{
								std::fprintf(stderr, "Line: %d ErrorCode: %d Message: %s\n", __LINE__, ec.value(),
									ec.message().c_str());
								::DeleteFile(newFileName.c_str());
								return;
							}
							::fprintf(stdout, "Transfer File: %s completed.\n", newFileName.c_str());
						});
						ftpContext->GetCtrlSession()->async_readuntil("\r\n", [this, ftpContext]
						{
							auto fun = [this, ftpContext] {
								ftpContext->_current_file.clear();
								epsv(ftpContext);
							};
							parse_response(ftpContext, "226", fun);
						}, std::bind(&State::session_err, this, ftpContext));
					}
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
}

void State::rnfr(std::shared_ptr<FtpContext> ftpContext)
{
}

void State::rnto(std::shared_ptr<FtpContext> ftpContext)
{
}

void State::nlst(std::shared_ptr<FtpContext> ftpContext)
{
}

void State::logout(std::shared_ptr<FtpContext> ftpContext)
{
}

RecvState & RecvState::Instance()
{
	static RecvState recvState;
	return recvState;
}

void RecvState::FileOP(std::shared_ptr<FtpContext> ftpContext)
{
	retr(ftpContext);
}

StorState & StorState::Instance()
{
	static StorState storState;
	return storState;
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

void NlstState::FileOP(std::shared_ptr<FtpContext> ftpContext)
{
	nlst(ftpContext);
}
