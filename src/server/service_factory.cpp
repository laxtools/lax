#include "stdafx.h"
#include <lax/server/service_factory.hpp>

namespace lax
{
namespace server
{

service_factory& service_factory::inst()
{
	static service_factory inst_; 

	return inst_;
}

void service_factory::add(const std::string& name, creator c)
{
	auto iter = map_.find(name);
	RETURN_IF(iter != map_.end());

	map_[name] = c; 
}

bool service_factory::has(const std::string& name)
{
	auto iter = map_.find(name);

	return iter != map_.end();
}

service_actor::ptr service_factory::create(
	const std::string& name, const nlm::json& config) const
{
	auto iter = map_.find(name);
	RETURN_IF(iter == map_.end(), service_actor::ptr());

	return iter->second(config);
}

} // net
} // lax