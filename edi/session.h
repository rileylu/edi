#pragma once
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>

#define TIMEOUT boost::posix_time::seconds(30)
class Session :boost::noncopyable, public std::enable_shared_from_this<Session> {
public:
	using Callback = std::function<void()>;
	Session(boost::asio::io_service &ios,
		const std::string& raw_ip_address,
		unsigned short port) :
		_sock(ios, boost::asio::ip::tcp::v4()),
		_ep(boost::asio::ip::address::from_string(raw_ip_address), port),
		_request_buf(std::make_shared<boost::asio::streambuf>()),
		_response_buf(std::make_shared<boost::asio::streambuf>()),
		_deadline(ios)
	{
		_sock.set_option(boost::asio::ip::tcp::no_delay(true));
		_sock.set_option(boost::asio::socket_base::reuse_address(true));
		_sock.set_option(boost::asio::socket_base::linger(true, 0));
		boost::asio::socket_base::non_blocking_io cmd(true);
		_sock.io_control(cmd);
	}
	~Session()
	{
		Close();
		_response_buf.reset();
		_request_buf.reset();
	}

	void async_connect(const Callback &callback);

	void async_send(const std::string& str, const Callback &callback);

	void async_send(const Callback &callback);

	void async_read(const Callback &callback);
	
	void async_read(boost::asio::windows::stream_handle &hd, Callback &callback);

	void async_readutil(const std::string& delim, const Callback &callback);

	boost::system::error_code Err() const {
		return _ec;
	}

	std::shared_ptr<boost::asio::streambuf> RequestBuf() const {
		return _request_buf;
	}
	std::shared_ptr<boost::asio::streambuf> ResponseBuf() const {
		return _response_buf;
	}

	void Cancel()
	{
		if (_sock.is_open())
			_sock.cancel(_ec);
	}
	void Close()
	{
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
		_deadline.async_wait(std::bind(&Session::check_deadline, shared_from_this()));
	}
private:
	boost::asio::ip::tcp::socket _sock;
	boost::asio::ip::tcp::endpoint _ep;
	std::shared_ptr<boost::asio::streambuf> _response_buf;
	std::shared_ptr<boost::asio::streambuf> _request_buf;
	boost::system::error_code _ec;
	boost::asio::deadline_timer _deadline;
};
