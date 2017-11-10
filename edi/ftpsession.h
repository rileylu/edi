#pragma once
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>

#define TIMEOUT boost::posix_time::seconds(30)

using Callback = std::function<void()>;
class FtpSession : boost::noncopyable, public std::enable_shared_from_this<FtpSession>
{
public:
	FtpSession(boost::asio::io_service& ios,
		const std::string& raw_ip_address,
		unsigned short port);

	~FtpSession();

	void Timeout();

	boost::asio::deadline_timer& Timer();
	template<typename P, typename N>
	void async_connect(P cb, N ecb);
	template<typename P, typename N>
	void async_readuntil(const std::string& delim, P p, N n);
	template<typename P, typename N>
	void async_send(const std::string& str, P p, N n);
	template <typename P, typename N>
	void async_read(P p, N n);
	template <typename Handler>
	bool transmit_file(std::string fn, Handler handler);

	boost::system::error_code Err() const;

	boost::asio::streambuf* RecvBuf() const;

	void NoWait();
	void Cancel();

	void Close();

private:
	void check_deadline(const boost::system::error_code&);

private:
	boost::asio::ip::tcp::socket _sock;
	boost::asio::windows::random_access_handle _file;
	boost::asio::ip::tcp::endpoint _ep;
	std::unique_ptr<boost::asio::streambuf> _rep;
	std::string _req;
	boost::system::error_code _ec;
	boost::asio::deadline_timer _deadline;

};

template<typename P, typename N>
inline void FtpSession::async_connect(P  cb, N  ecb)
{
	_deadline.expires_from_now(TIMEOUT);
	auto t = shared_from_this();
	_sock.async_connect(_ep, [t, cb, ecb](const boost::system::error_code& ec)
	{
		if (ec)
		{
			t->_ec = ec;
			ecb();
			t->NoWait();
			return;
		}
		cb();
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, shared_from_this(), std::placeholders::_1));
}

template<typename P, typename N>
inline void FtpSession::async_readuntil(const std::string& delim, P  p, N  n)
{
	_deadline.expires_from_now(TIMEOUT);
	auto t = shared_from_this();
	boost::asio::async_read_until(_sock, *_rep, delim,
		[t, p, n](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			t->_ec = ec;
			n();
			t->NoWait();
			return;
		}
		p();
	});
}

template<typename P, typename N>
inline void FtpSession::async_send(const std::string & str, P  p, N  n)
{
	_deadline.expires_from_now(TIMEOUT);
	auto t = shared_from_this();
	_req = std::move(str);
	boost::asio::async_write(_sock, boost::asio::buffer(_req),
		[t, p, n](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			t->_ec = ec;
			n();
			t->NoWait();
			return;
		}
		p();
	});

}

template<typename P, typename N>
inline void FtpSession::async_read(P  p, N  n)
{
	_deadline.expires_from_now(TIMEOUT);
	auto t = shared_from_this();
	boost::asio::async_read(_sock, *_rep,
		[t, p, n](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			t->_ec = ec;
			n();
			t->NoWait();
			return;
		}
		p();
	});
}

template <typename Handler>
inline bool FtpSession::transmit_file(std::string fn, Handler handler)
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
inline void FtpSession::Timeout()
{
	_deadline.cancel(_ec);
}

inline boost::asio::deadline_timer & FtpSession::Timer()
{
	return _deadline;
}

inline boost::system::error_code FtpSession::Err() const
{
	return _ec;
}

inline boost::asio::streambuf* FtpSession::RecvBuf() const
{
	return _rep.get();
}

inline void FtpSession::NoWait()
{
	_deadline.expires_at(boost::posix_time::pos_infin);
}

inline void FtpSession::Cancel()
{
	_sock.cancel(_ec);
	_file.cancel(_ec);
	_deadline.cancel(_ec);
}

inline void FtpSession::Close()
{
	_rep.reset(nullptr);
	_sock.shutdown(_sock.shutdown_both, _ec);
	_sock.close(_ec);
	_file.close(_ec);
	_deadline.cancel(_ec);
}

inline void FtpSession::check_deadline(const boost::system::error_code &)
{
	if (_deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		_sock.shutdown(boost::asio::socket_base::shutdown_both, _ec);
		_sock.close(_ec);
		NoWait();
		return;
	}
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, shared_from_this(), std::placeholders::_1));
}
