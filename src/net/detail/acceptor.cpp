#include "stdafx.h"

#include <lax/net/detail/acceptor.hpp>
#include <lax/net/detail/service_impl.hpp>

namespace lax
{
namespace net
{

acceptor::acceptor(uint16_t id, const std::string& protocol, const std::string& addr)
	: id_(id)
	, addr_(addr)
	, protocol_(protocol)
	, socket_(service::inst().impl().get_ios())
	, acceptor_(service::inst().impl().get_ios())
{
}

acceptor::~acceptor()
{
}

service::result acceptor::listen()
{
	return_if(
		!addr_.is_valid(),
		service::result(false, reason::fail_invalid_address)
	);

	try
	{
		acceptor_.open(addr_.get_endpoint().protocol());
	}
	catch (asio::system_error& se)
	{
		util::log()->error(
			"acceptor failed to open. endpoint: {0}, reason: {1}", 
			addr_.get_raw(),  
			se.what()
		);

		return service::result(true, reason::fail_acceptor_open);
	}

	try
	{
		acceptor_.bind(addr_.get_endpoint());
	}
	catch (asio::system_error& se)
	{
		util::log()->error(
			"acceptor failed to bind. endpoint: {0}, reason: {1}",
			addr_.get_raw(),
			se.what()
		);

		return service::result(true, reason::fail_acceptor_bind);
	}

	try
	{
		acceptor_.listen();
	}
	catch (asio::system_error& se)
	{
		util::log()->error(
			"acceptor failed to listen. endpoint: {0}, reason: {1}",
			addr_.get_raw(),
			se.what()
		);

		return service::result(true, reason::fail_acceptor_listen);
	}

	do_accept();

	return service::result(true, reason::success);
}

void acceptor::do_accept()
{
	return_if(!acceptor_.is_open());

	acceptor_.async_accept(
		socket_,
		[this](const asio::error_code& ec) {this->on_accepted(ec);}
	);
}

void acceptor::on_accepted(const asio::error_code& ec)
{
	if (!ec) 
	{
		service::inst().impl().on_accepted(id_, std::move(socket_));
	}
	else
	{
		util::log()->error(
			"acceptor failed to accept. endpoint: {0}, reason: {1}",
			addr_.get_raw(),
			ec.message()
		);

		service::inst().impl().on_accept_failed(id_, ec);
	}

	do_accept();
}

} // net
} // lax
