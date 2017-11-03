#include "ftpsession.h"

void FtpSession::async_connect(const PositiveCallback& callback, const NegitiveCallback& err)
{
	_deadline.expires_from_now(TIMEOUT);
	auto t = shared_from_this();
	_sock.async_connect(_ep, [t, callback,err](const boost::system::error_code& ec)
	{
		if (ec)
		{
			t->_ec = ec;
			err();
			return;
		}
		t->NoWait();
		callback(0);
	});
	_deadline.async_wait(std::bind(&FtpSession::check_deadline, shared_from_this(),std::placeholders::_1));
}

void FtpSession::async_send(std::string str, const PositiveCallback& callback, const NegitiveCallback& err)
{
	auto t = shared_from_this();
	_req = str;
	boost::asio::async_write(_sock, boost::asio::buffer(_req),
	                         [t,callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                         {
		                         if (ec)
		                         {
			                         t->_ec = ec;
			                         err();
			                         return;
		                         }
		                         callback(bytes_transferred);
	                         });
}


void FtpSession::async_read(const PositiveCallback& callback, const NegitiveCallback& err)
{
	auto t = shared_from_this();
	boost::asio::async_read(_sock, *_rep,
	                        [t, callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                        {
		                        if (ec)
		                        {
			                        t->_ec = ec;
			                        err();
			                        return;
		                        }
		                        callback(bytes_transferred);
	                        });
}

void FtpSession::async_readutil(const std::string& delim, const PositiveCallback& callback, const NegitiveCallback& err)
{
	auto t = shared_from_this();
	boost::asio::async_read_until(_sock, *_rep, delim,
	                              [t, callback,err](const boost::system::error_code& ec, std::size_t bytes_transferred)
	                              {
		                              if (ec)
		                              {
			                              t->_ec = ec;
			                              err();
			                              return;
		                              }
		                              callback(bytes_transferred);
	                              });
}
