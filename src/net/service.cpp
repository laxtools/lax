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
{
}

service::~service()
{
}

bool service::start()
{
	// service is a singleton, but intialized once here
	// - impl_ is shared like a singleton
	// - start and finish can be called multiple times 
	//   - not like a singleton
	//

	std::lock_guard<std::recursive_mutex> lock(mutex_);

	if (!impl_)
	{
		impl_ = std::make_unique<service_impl>(*this);

		return impl_->start();
	}

	return impl_->is_running();
}

void service::finish()
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	impl_->finish();
	impl_.reset(); 
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

bool service::is_running() const
{
	return impl_->is_running();
}

uint16_t service::get_acceptor_count() const
{
	return impl_->get_acceptor_count();
}

uint16_t service::get_connector_count() const
{
	return impl_->get_connector_count();
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
