#pragma once
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
		_ep(boost::asio::ip::address::from_string(raw_ip_address), port),
		_buf(std::make_shared<boost::asio::streambuf>()),
		_deadline(ios)
	{
		_sock.set_option(boost::asio::ip::tcp::no_delay(true));
		_sock.set_option(boost::asio::socket_base::reuse_address(true));
		_sock.set_option(boost::asio::socket_base::linger(true, 0));
		_sock.set_option(boost::asio::socket_base::keep_alive(true));
		boost::asio::socket_base::non_blocking_io cmd(true);
		_sock.io_control(cmd);
	}

	~FtpSession()
	{
		Close();
		_buf.reset();
	}

	void Timeout()
	{
		_deadline.cancel(_ec);
	}

	void async_connect(const PositiveCallback& callback, const NegitiveCallback& err);

	void async_send(const std::string& str, const PositiveCallback& callback, const NegitiveCallback& err);
	void async_send(std::shared_ptr<boost::asio::streambuf> buf, const PositiveCallback& callback,
	                const NegitiveCallback& err);

	void async_read(const PositiveCallback& callback, const NegitiveCallback& err);

	void async_readutil(const std::string& delim, const PositiveCallback& callback, const NegitiveCallback& err);

	boost::system::error_code Err() const
	{
		return _ec;
	}

	std::shared_ptr<boost::asio::streambuf>& RecvBuf()
	{
		return _buf;
	}

	void Cancel()
	{
		if (_sock.is_open())
			_sock.cancel(_ec);
	}

	void Close()
	{
		_buf.reset();
		_sock.shutdown(_sock.shutdown_both, _ec);
		_sock.close(_ec);
	}

private:
	void check_deadline()
	{
		if (_deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			Close();
			return;
		}
		_deadline.async_wait(std::bind(&FtpSession::check_deadline, shared_from_this()));
	}

private:
	boost::asio::ip::tcp::socket _sock;
	boost::asio::ip::tcp::endpoint _ep;
	std::shared_ptr<boost::asio::streambuf> _buf;
	boost::system::error_code _ec;
	boost::asio::deadline_timer _deadline;
};
