#include "ftpsession.h"

FtpSession::FtpSession(boost::asio::io_service & ios, const std::string & raw_ip_address, unsigned short port) :
	_sock(ios, boost::asio::ip::tcp::v4()),
	_file(ios),
	_ep(boost::asio::ip::address::from_string(raw_ip_address), port),
	_rep(std::make_shared<boost::asio::streambuf>()),
	_deadline(ios)
{
	_sock.set_option(boost::asio::socket_base::reuse_address(true));
	_sock.set_option(boost::asio::socket_base::linger(true, 0));
	boost::asio::socket_base::non_blocking_io cmd(true);
	_sock.io_control(cmd);
}

FtpSession::~FtpSession()
{
	Close();
}