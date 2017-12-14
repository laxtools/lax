#include "stdafx.h"
#include <lax/channel/detail/channel_map.hpp>

namespace lax
{
namespace channel
{

channel_map& channel_map::get()
{
	static channel_map inst;

	return inst;
}

channel_map::channel_map()
{
}

channel_map::~channel_map()
{
	map_.clear();
}

channel_map::ptr channel_map::create(const key_t& key, const config& cfg)
{
	// shared lock. check
	{
		std::shared_lock<std::shared_timed_mutex> lock(mutex_);

		auto iter = map_.find(key);
		check(iter == map_.end());
		return_if(iter != map_.end(), iter->second);
	}

	auto cp = std::make_shared<channel>(key, cfg);

	// exclusive lock. insert
	{
		std::unique_lock<std::shared_timed_mutex> lock(mutex_);
		map_.insert(map::value_type(key, cp));
	}

	return cp;
}

channel_map::ptr channel_map::find(const key_t& key)
{
	std::shared_lock<std::shared_timed_mutex> lock(mutex_);

	auto iter = map_.find(key);
	return_if(iter == map_.end(), ptr());

	return iter->second;
}

void channel_map::destroy(const key_t& key)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto iter = map_.find(key);
	check(iter != map_.end());
	return_if(iter == map_.end());

	map_.erase(iter);
}

} // channel 
} // lax