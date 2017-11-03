﻿#pragma once
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>

#define TIMEOUT boost::posix_time::seconds(120)

class FtpSession : boost::noncopyable, public std::enable_shared_from_this<FtpSession>
{
public:
	using PositiveCallback = std::function<void(std::size_t)>;
	using NegitiveCallback = std::function<void()>;

	FtpSession(boost::asio::io_service& ios,
	           const std::string& raw_ip_address,
	           unsigned short port) :
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

	~FtpSession()
	{
		Close();
	}

	void Timeout()
	{
		_deadline.cancel(_ec);
	}

	boost::asio::deadline_timer& Timer()
	{
		return _deadline;
	}

	void async_connect(const PositiveCallback& callback, const NegitiveCallback& err);

	void async_send(std::string str, const PositiveCallback& callback, const NegitiveCallback& err);

	void async_read(const PositiveCallback& callback, const NegitiveCallback& err);

	void async_readutil(const std::string& delim, const PositiveCallback& callback, const NegitiveCallback& err);

	template <typename Handler>
	bool transmit_file(std::string fn, Handler handler);

	boost::system::error_code Err() const
	{
		return _ec;
	}

	std::shared_ptr<boost::asio::streambuf>& RecvBuf()
	{
		return _rep;
	}

	void NoWait()
	{
		_deadline.expires_at(boost::posix_time::pos_infin);
	}
	void Cancel()
	{
		if (_sock.is_open())
			_sock.cancel(_ec);
	}

	void Close()
	{
		_rep.reset();
		_sock.shutdown(_sock.shutdown_both, _ec);
		_sock.close(_ec);
		_file.close(_ec);
	}

private:
	void check_deadline(const boost::system::error_code&)
	{
		if (_deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			_sock.close(_ec);
			NoWait();
		}
		//_deadline.async_wait(std::bind(&FtpSession::check_deadline, shared_from_this(), std::placeholders::_1));
	}

private:
	boost::asio::ip::tcp::socket _sock;
	boost::asio::windows::random_access_handle _file;
	boost::asio::ip::tcp::endpoint _ep;
	std::shared_ptr<boost::asio::streambuf> _rep;
	std::string _req;
	boost::system::error_code _ec;
	boost::asio::deadline_timer _deadline;
};

template <typename Handler>
bool FtpSession::transmit_file(std::string fn, Handler handler)
{
	boost::system::error_code ec;
	_file.assign(::CreateFile(fn.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
	                          0), ec);
	if (_file.is_open())
	{
		boost::asio::windows::overlapped_ptr overlapped(_sock.get_io_service(), handler);
		BOOL ok = ::TransmitFile(_sock.native(), _file.native(), 0, 0, overlapped.get(), 0, 0);
		DWORD last_error = ::GetLastError();
		if (!ok && last_error != ERROR_IO_PENDING)
		{
			boost::system::error_code ec(last_error, boost::asio::error::get_system_category());
			overlapped.complete(ec, 0);
		}
		else
		{
			overlapped.release();
		}
		return true;
	}
	_file.close(_ec);
	return false;
}
