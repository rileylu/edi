#pragma once
#include <boost/asio.hpp>
#include <string>
#include <mutex>
using namespace boost;

struct Session {
	Session(asio::io_service &ios,
		const std::string& raw_ip_address,
		unsigned short port) :
		_sock(ios),
		_ep(asio::ip::address::from_string(raw_ip_address), port)
	{}
	asio::ip::tcp::socket _sock;
	asio::ip::tcp::endpoint _ep;

	asio::streambuf _response_buf;
	//asio::streambuf _request_buf;
	std::string _request_buf;
	system::error_code _ec;
	bool _was_cancelled;
	std::mutex _cancel_guard;

};
//
//class TCPStream {
//public:
//private:
//	asio::ip::tcp::socket _sock;
//	asio::ip::tcp::endpoint _ep;
//	asio::streambuf _response_buf;
//	asio::streambuf _request_buf;
//	system::error_code _ec;
//
//};