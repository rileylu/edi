#include "state.h"
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>


using namespace boost;

void ConnectionClosedState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string& filename)
{
	auto handler = [ftpContext, filename, this](const boost::system::error_code& ec) {
		if (ec)
		{
			std::cerr << "connection error" << std::endl;
			return;
		}
		asio::async_read_until(ftpContext->GetCtrlSession()->_sock, ftpContext->GetCtrlSession()->_response_buf, "\r\n", [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
			if (ec)
			{
				return;
			}
			std::istream is(&ftpContext->GetCtrlSession()->_response_buf);
			std::string res;
			while (std::getline(is, res))
			{
				std::cout << res << std::endl;
				if (res[3] == '-')
					continue;
				else if (res.find("220") != res.npos)
				{
					auto& connectionReadyState = ConnectionReadyState::Instance();
					ChangeStatus(ftpContext, &connectionReadyState);
					ftpContext->DoSendFile(filename);
				}
				else
					return;
			}
		});
	};
	ftpContext->GetCtrlSession()->_sock.async_connect(ftpContext->GetCtrlSession()->_ep, handler);
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
	ftpContext->GetCtrlSession()->_request_buf = std::move(request);
	auto user = [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			return;
		}
		asio::async_read_until(ftpContext->GetCtrlSession()->_sock, ftpContext->GetCtrlSession()->_response_buf, "\r\n", [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
			if (ec)
			{
				std::cerr << ec.message() << std::endl;
				return;
			}
			std::istream is(&ftpContext->GetCtrlSession()->_response_buf);
			std::string res;
			while (std::getline(is, res))
			{
				std::cout << res << std::endl;
				if (res[3] == '-')
					continue;
				else if (res.find("331") != res.npos)
				{
					std::string request;
					request += "PASS ";
					request += ftpContext->GetPWD();
					request += "\r\n";
					ftpContext->GetCtrlSession()->_request_buf = std::move(request);
					asio::async_write(ftpContext->GetCtrlSession()->_sock, asio::buffer(ftpContext->GetCtrlSession()->_request_buf), [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
						if (ec)
						{
							std::cerr << ec.message() << std::endl;
							return;
						}
						asio::async_read_until(ftpContext->GetCtrlSession()->_sock, ftpContext->GetCtrlSession()->_response_buf, "\r\n", [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
							if (ec)
							{
								std::cerr << ec.message() << std::endl;
								return;
							}
							std::istream is(&ftpContext->GetCtrlSession()->_response_buf);
							std::string res;
							while (std::getline(is, res))
							{
								std::cout << res << std::endl;
								if (res[3] == '-')
									continue;
								else if (res.find("230") != res.npos)
								{
									auto& loginReadyState = LoginReadyState::Instance();
									ChangeStatus(ftpContext, &loginReadyState);
									ftpContext->DoSendFile(filename);
								}
								else
									return;
							}


						});
					});
				}
				else
					return;
			}
		});

	};
	asio::async_write(ftpContext->GetCtrlSession()->_sock, asio::buffer(ftpContext->GetCtrlSession()->_request_buf), user);
}

void LoginReadyState::DoSendFile(std::shared_ptr<FtpContext> ftpContext, const std::string & filename)
{
	std::string request = "EPSV\r\n";
	ftpContext->GetCtrlSession()->_request_buf = std::move(request);
	asio::async_write(ftpContext->GetCtrlSession()->_sock, asio::buffer(ftpContext->GetCtrlSession()->_request_buf), [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			return;
		}
		asio::async_read_until(ftpContext->GetCtrlSession()->_sock, ftpContext->GetCtrlSession()->_response_buf, "\r\n", [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
			if (ec)
			{
				std::cerr << ec.message() << std::endl;
				return;
			}
			std::istream is(&ftpContext->GetCtrlSession()->_response_buf);
			std::string res;
			while (std::getline(is, res))
			{
				std::cout << res << std::endl;
				if (res[3] == '-')
					continue;
				else if (res.find("229") != res.npos)
				{
					std::regex port_regex(R"(.*\|{3}([0-9]+)\|{1}.*)");
					std::smatch results;
					unsigned short port = 0;
					if (std::regex_search(res, results, port_regex))
					{
						try
						{
							port = std::stoul(std::string(results[1].first, results[1].second));
						}
						catch (const std::invalid_argument &e)
						{
							continue;
						}
					}
					else
					{
						return;
					}
					ftpContext->BuildDataSession(port);
					ftpContext->GetDataSession()->_sock.async_connect(ftpContext->GetDataSession()->_ep, [this, ftpContext, filename](const system::error_code& ec) {
						if (ec)
						{
							std::cerr << ec.message() << std::endl;
							ftpContext->DoSendFile(filename);
							return;
						}
						auto& readyForTransferState = ReadyForTransferState::Instance();
						ChangeStatus(ftpContext, &readyForTransferState);
						ftpContext->DoSendFile(filename);
					});
				}
				else
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
	ftpContext->GetCtrlSession()->_request_buf = std::move(cmd);
	asio::async_write(ftpContext->GetCtrlSession()->_sock, asio::buffer(ftpContext->GetCtrlSession()->_request_buf), [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			return;
		}
		std::function<void(const system::error_code&, std::size_t)>  downloadFile = [this, ftpContext, filename, &downloadFile](const system::error_code& ec, std::size_t bytes_transferred) {
			if (ec.value() == 2)
			{
				std::istream is(&ftpContext->GetDataSession()->_response_buf);
				asio::streambuf::const_buffers_type cbt = ftpContext->GetDataSession()->_response_buf.data();
				std::string res(asio::buffers_begin(cbt), asio::buffers_end(cbt));
				std::string newFilename = std::move(filename);
				newFilename.erase(0, newFilename.find_last_of('/') + 1);
				std::ofstream of(newFilename);
				std::copy(asio::buffers_begin(cbt), asio::buffers_end(cbt), std::ostream_iterator<char>(of));
				of.flush();
				of.close();
				ftpContext->GetDataSession()->_sock.close();
				asio::async_read_until(ftpContext->GetCtrlSession()->_sock, ftpContext->GetCtrlSession()->_response_buf, "\r\n", [this, ftpContext, filename](const system::error_code& ec, std::size_t bytes_transferred) {
					if (ec)
					{
						std::cerr << ec.message() << std::endl;
						return;
					}
					std::istream in(&ftpContext->GetCtrlSession()->_response_buf);
					std::string res;
					std::getline(in, res);
					if (res.find("150") == 0)
					{
						asio::async_read_until(ftpContext->GetCtrlSession()->_sock, ftpContext->GetCtrlSession()->_response_buf, "\r\n", [this, ftpContext, filename](const system::error_code &ec, std::size_t bytes_transferred) {
							if (ec)
							{
								std::cerr << ec.message() << std::endl;
								return;
							}
							std::istream is(&ftpContext->GetCtrlSession()->_response_buf);
							std::string res;
							getline(is, res);
							std::cout << res << std::endl;
							if (res.find("226") == 0)
							{
								ChangeStatus(ftpContext, &LoginReadyState::Instance());
								std::string *fn = nullptr;
								if (ftpContext->_fileList->pop(fn))
									ftpContext->DoSendFile(std::string(*fn));
								delete fn;
							}
							else
							{
								return;
							}

						});

					}
				});
				return;
			}
		};
		asio::async_read(ftpContext->GetDataSession()->_sock, ftpContext->GetDataSession()->_response_buf, downloadFile);
	});
}

void ReadyForTransferState::DoList(std::shared_ptr<FtpContext> ftpContext, const std::string & dir)
{
}
