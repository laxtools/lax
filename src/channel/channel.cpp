#include "stdafx.h"
#include <lax/channel/channel.hpp>
#include <lax/channel/detail/channel_map.hpp>

namespace lax
{
namespace channel
{

channel::ptr channel::create(const key_t& key, const config& cfg)
{
	return channel_map::get().create(key, cfg);
}

channel::ptr channel::find(const key_t& key)
{
	return channel_map::get().find(key);
}

void channel::destroy(const key_t& key)
{
	return channel_map::get().destroy(key);
}

channel::channel(const key_t& key, const config& cfg)
	: key_(key)
	, config_(cfg)
	, map_(config_.use_lock)
{
}

channel::~channel()
{
	if (config_.post_all_when_destroyed)
	{
		while (q_.unsafe_size() > 0)
		{
			post();
		}
	}
}

std::size_t channel::push(message::ptr m)
{
	q_.push(m);
	
	auto count = map_.post(m, sub::mode::immediate);

	stat_.total_immediate_post_count += count;

	return count;
}

std::size_t channel::push(message::topic_t topic, message::ptr m)
{
	q_.push(m);

	auto count = map_.post(topic, m, sub::mode::immediate);

	stat_.total_immediate_post_count += count;

	return count;
}

sub::key_t channel::subscribe(message::topic_t topic, cond_t cond, cb_t cb, sub::mode mode)
{
	return map_.subscribe(topic, cond, cb, mode);
}

sub::key_t channel::subscribe(message::topic_t topic, cb_t cb, sub::mode mode)
{
	return map_.subscribe(topic, cb, mode);
}

void channel::unsubscribe(sub::key_t key)
{
	map_.unsubscribe(key);
}

std::size_t channel::post()
{
	message::ptr m;

	std::size_t count = 0;

	while (q_.pop(m))
	{
		count += map_.post(m, sub::mode::delayed);
	}

	stat_.last_post_count = count;
	stat_.total_post_count += stat_.last_post_count;

	return count;
}

std::size_t channel::get_queue_size() const
{
	return q_.unsafe_size();
}

} // channel
} // lax
