#include "stdafx.h"

#include <lax/net/detail/connector.hpp>
#include <lax/net/detail/service_impl.hpp>

namespace lax
{
namespace net
{

connector::connector(uint16_t id, const std::string& protocol, const std::string& addr)
	: id_(id)
	, protocol_(protocol)
	, addr_(addr)
	, socket_(service::inst().impl().get_ios())
{
}

connector::~connector()
{
}

service::result connector::connect()
{
	// �ǵ������� callback���� �����ϰ� ����

	socket_.async_connect(
		addr_.get_endpoint(), 
		[this](const asio::error_code& ec) { on_connected(ec); }
	);

	return service::result(true, success);
}

void connector::on_connected(const asio::error_code& ec)
{
	if (!ec) // �� ���� ����
	{
		service::inst().impl().on_connected(id_, std::move(socket_));
	}
	else
	{
		util::log()->error(
			"connector failed to connect. endpoint: {0}, reason: {1}",
			addr_.get_raw(),
			ec.message()
		);

		service::inst().impl().on_connect_failed(id_, ec);
	}
}

} // net
} // lax
