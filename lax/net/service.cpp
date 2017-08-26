#include "stdafx.h"

#include "service.h"
#include "detail/service_impl.h"

namespace lax
{
namespace net
{

service::service(app* ap)
	: impl_(new service_impl(ap, *this))
{
}

service::~service()
{
	impl_->stop(); // stop and wait
}

service::result service::listen(const std::string& addr, creator& c)
{
	return impl_->listen(addr, c);
}

service::result service::connect(const std::string& addr, creator& c)
{
	return impl_->connect(addr, c);
}

service::result service::send(const session::id& id, uint8_t* data, std::size_t len)
{
	return impl_->send(id, data, len);
}

void service::close(const session::id& id)
{
	return impl_->close(id);
}

void service::error(const session::id& id)
{
	return impl_->error(id);
}

app* service::get_app()
{
	return impl_->get_app();
}

session::ptr service::get(const session::id& id)
{
	return impl_->get(id);
}

} // net
} // lax
