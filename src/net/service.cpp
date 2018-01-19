#include "stdafx.h"

#include <lax/net/service.hpp>
#include <lax/net/detail/service_impl.hpp>
#include <chrono>

namespace lax
{
namespace net
{

service::config service::cfg;

service& service::inst()
{
	static service inst_;

	return inst_;
}

service::service()
	: impl_(new service_impl(*this))
{
}

service::~service()
{
	impl_->fini(); // stop and wait
}

bool service::init()
{
	return impl_->init();
}

void service::fini()
{
	impl_->fini();
}

void service::wait(unsigned int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

service::result service::listen(const std::string& addr, const std::string& protocol)
{
	return impl_->listen(addr, protocol);
}

service::result service::connect(const std::string& addr, const std::string& protocol)
{
	return impl_->connect(addr, protocol);
}

session::ref service::acquire(const session::id& id)
{
	return impl_->acquire(id);
}

std::size_t service::get_session_count() const
{
	return impl_->get_session_count();
}

void service::error(const session::id& id)
{
	return impl_->error(id);
}

} // net
} // lax
