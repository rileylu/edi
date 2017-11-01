#include "session.h"

void Session::async_connect(const Callback &callback)
{
	_deadline.expires_from_now(TIMEOUT);
	_sock.async_connect(_ep, [this, callback](const boost::system::error_code& ec) {
		if (ec)
		{
			_ec = ec;
		}
		callback();
	});
	_deadline.async_wait(std::bind(&Session::check_deadline, shared_from_this()));
}

void Session::async_send(const std::string& str, const Callback& callback)
{
	_request_buf->sputn(str.c_str(), str.size());
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_write(_sock, *_request_buf, [this, callback](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			_ec = ec;
		}
		callback();
	});
}

void Session::async_send(const Callback& callback)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_write(_sock, *_request_buf, [this, callback](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			_ec = ec;
		}
		callback();
	});
}


void Session::async_read(const Callback &callback)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_read(_sock, *_response_buf, [this, callback](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			_ec = ec;
		}
		callback();
	});
}
void Session::async_read(std::shared_ptr<boost::asio::windows::stream_handle> hd, const Callback & callback)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_read(*hd, *_request_buf, [this, callback](const boost::system::error_code& ec, std::size_t) {
		if (ec)
		{
			_ec = ec;
		}
		callback();
	});
}
void Session::async_readutil(const std::string & delim, const Callback& callback)
{
	_deadline.expires_from_now(TIMEOUT);

	boost::asio::async_read_until(_sock, *_response_buf, delim, [this, callback](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			_ec = ec;
		}
		callback();
	});
}
