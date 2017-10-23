#pragma once
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <mutex>


class Session :boost::noncopyable {
public:
	Session(boost::asio::io_service &ios,
		const std::string& raw_ip_address,
		unsigned short port) :
		_sock(ios),
		_ep(boost::asio::ip::address::from_string(raw_ip_address), port)
	{}


	template<typename Fun, typename...Args>
	void async_connect(Fun&& f, Args&&... args);

	template<typename Fun, typename... Args>
	void async_send(const std::string& str, Fun&& f, Args&&... args);

	template<typename Fun, typename... Args>
	void async_read(Fun&& f, Args&&... args);

	template<typename Fun, typename... Args>
	void async_readutil(const std::string& delim, Fun&& fun, Args&&... args);

	void Close() {
		_sock.close();
	}

	boost::system::error_code Err() const {
		return _ec;
	}

	boost::asio::streambuf* RequestBuf() {
		return &_request_buf;
	}
	boost::asio::streambuf* ResponseBuf() {
		return &_response_buf;
	}
private:
	boost::asio::ip::tcp::socket _sock;
	boost::asio::ip::tcp::endpoint _ep;
	boost::asio::streambuf _response_buf;
	boost::asio::streambuf _request_buf;
	boost::system::error_code _ec;
};

template<typename Fun, typename ...Args>
void Session::async_connect(Fun &&f, Args&&... args)
{
	_sock.async_connect(_ep, [this, f, args...](const boost::system::error_code& ec) {
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			_ec = ec;
			_sock.close();
		}
		f(args...);
	});
}

template<typename Fun, typename ...Args>
void Session::async_send(const std::string& str, Fun && f, Args && ...args)
{
	_request_buf.sputn(str.c_str(), str.size());
	asio::async_write(_sock, _request_buf, [this, f, args...](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			_sock.close();
			_ec = ec;
		}
		f(args...);
	});
}


template<typename Fun, typename ...Args>
void Session::async_read(Fun && f, Args && ...args)
{
	boost::asio::async_read(_sock, _response_buf, [this, f, args...](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			if (ec.value() == 2)
				std::cerr << ec.message() << std::endl;
			_sock.close();
			_ec = ec;
		}
		f(args...);
	});
}
template<typename Fun, typename ...Args>
void Session::async_readutil(const std::string & delim, Fun && f, Args && ...args)
{
	boost::asio::async_read_until(_sock, _response_buf, delim, [this, f, args...](const boost::system::error_code& ec, std::size_t bytes_transferred) {
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			_sock.close();
			_ec = ec;
		}
		f(args...);
	});
}

