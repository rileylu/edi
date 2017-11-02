#include "ftpsession.h"

void FtpSession::async_connect(const PositiveCallback& callback, const NegitiveCallback& err)
{
	_deadline.expires_from_now(TIMEOUT);
	_sock.async_connect(_ep, [this, callback,err](const boost::system::error_code& ec)
	{
		if (ec)
		{
			_ec = ec;
			err();
			return;
		}
		callback(0);
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, shared_from_this()));
}

void FtpSession::async_send(const std::string& str, const PositiveCallback& callback, const NegitiveCallback& err)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_write(_sock, boost::asio::buffer(str),
	                         [this, callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                         {
		                         if (ec)
		                         {
			                         _ec = ec;
			                         err();
			                         return;
		                         }
		                         callback(bytes_transferred);
	                         });
}

void FtpSession::async_send(std::shared_ptr<boost::asio::streambuf> buf, const PositiveCallback& callback,
                            const NegitiveCallback& err)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_write(_sock, *buf,
	                         [this, callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                         {
		                         if (ec)
		                         {
			                         _ec = ec;
			                         err();
			                         return;
		                         }
		                         callback(bytes_transferred);
	                         });
}


void FtpSession::async_read(const PositiveCallback& callback, const NegitiveCallback& err)
{
	_deadline.expires_from_now(TIMEOUT);
	boost::asio::async_read(_sock, *_buf,
	                        [this, callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                        {
		                        if (ec)
		                        {
			                        _ec = ec;
			                        err();
			                        return;
		                        }
		                        callback(bytes_transferred);
	                        });
}

void FtpSession::async_readutil(const std::string& delim, const PositiveCallback& callback, const NegitiveCallback& err)
{
	_deadline.expires_from_now(TIMEOUT);

	boost::asio::async_read_until(_sock, *_buf, delim,
	                              [this, callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                              {
		                              if (ec)
		                              {
			                              _ec = ec;
			                              err();
			                              return;
		                              }
		                              callback(bytes_transferred);
	                              });
}
