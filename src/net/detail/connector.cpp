#include "stdafx.h"

#include "connector.h"
#include <lax/net/detail/service_impl.h>

namespace lax
{
namespace net
{

connector::connector(service* svc, uint16_t id, service::creator& creator, const std::string& addr)
	: svc_(svc)
	, id_(id)
	, creator_(creator)
	, addr_(addr)
	, socket_(svc_->impl()->get_ios())
{
}

connector::~connector()
{
}

service::result connector::connect()
{
	return_if(
		!addr_.is_valid(),
		service::result(false, reason::fail_invalid_address)
	);

	socket_.async_connect(
		addr_.get_endpoint(), 
		[this](const asio::error_code& ec) { on_connected(ec); }
	);

	return service::result(true, success);
}

session::ptr connector::create(const session::id& id, tcp::socket&& soc)
{
	return creator_(*svc_, id, std::move(soc), false);
}

void connector::on_connected(const asio::error_code& ec)
{
	if (!ec) // 이 쪽이 성공
	{
		svc_->impl()->on_connected(id_, std::move(socket_));
	}
	else
	{
		util::log_helper::get()->error(
			"connector failed to connect. endpoint: {0}, reason: {1}",
			addr_.get_raw(),
			ec.message()
		);

		svc_->impl()->on_connect_failed(id_);
	}
}

} // net
} // lax
