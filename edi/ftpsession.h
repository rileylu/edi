#pragma once
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <boost/asio/error.hpp>
#include <boost/asio/steady_timer.hpp>

#define TIMEOUT std::chrono::seconds(30)

using Callback = std::function<void()>;
class FtpSession : boost::noncopyable
{
public:
	FtpSession(boost::asio::io_service& ios,
		const std::string& raw_ip_address,
		unsigned short port);

	~FtpSession();

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

	boost::asio::steady_timer& Timer();

	boost::system::error_code& Err();

	boost::asio::streambuf* RecvBuf() const;
	std::shared_ptr<boost::asio::streambuf> GetSharedRecvBuf();

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
	boost::asio::steady_timer _deadline;

};

template<typename P, typename N>
inline void FtpSession::async_connect(P  cb, N  ecb)
{
	_deadline.expires_from_now(TIMEOUT);
	_sock.async_connect(_ep, [this, cb, ecb](const boost::system::error_code& ec)
	{
		if (ec)
		{
			_ec = ec;
			ecb();
			return;
		}
		cb();
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, this, std::placeholders::_1));
}

template<typename P, typename N>
inline void FtpSession::async_readuntil(const std::string& delim, P  p, N  n)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_read_until(_sock, *_rep, delim,
		[this, p, n](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			_ec = ec;
			n();
			return;
		}
		p();
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, this, std::placeholders::_1));
}

template<typename P, typename N>
inline void FtpSession::async_send(const std::string & str, P  p, N  n)
{
	_deadline.expires_from_now(TIMEOUT);
	_req = std::move(str);
	boost::asio::async_write(_sock, boost::asio::buffer(_req),
		[this, p, n](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			_ec = ec;
			n();
			return;
		}
		p();
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, this, std::placeholders::_1));

}

template<typename P, typename N>
inline void FtpSession::async_read(P  p, N  n)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_read(_sock, *_rep,
		[this, p, n](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			_ec = ec;
			n();
			return;
		}
		p();
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, this, std::placeholders::_1));
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

inline boost::asio::steady_timer & FtpSession::Timer()
{
	return _deadline;
}

inline boost::system::error_code& FtpSession::Err()
{
	return _ec;
}

inline boost::asio::streambuf* FtpSession::RecvBuf() const
{
	return _rep.get();
}

inline std::shared_ptr<boost::asio::streambuf> FtpSession::GetSharedRecvBuf()
{
	boost::asio::streambuf *p = _rep.release();
	return std::shared_ptr<boost::asio::streambuf>(p);
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
	if (_sock.is_open())
	{
		_sock.shutdown(_sock.shutdown_both, _ec);
		_sock.close(_ec);
	}
	_file.close(_ec);
	_deadline.cancel(_ec);
}

inline void FtpSession::check_deadline(const boost::system::error_code &ec)
{
	if (ec == 0)
	{
		_ec = boost::asio::error::operation_aborted;
		_sock.shutdown(boost::asio::socket_base::shutdown_both, _ec);
		_sock.close(_ec);
	}
}
