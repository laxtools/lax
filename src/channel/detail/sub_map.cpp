#include "stdafx.h"
#include <lax/channel/detail/sub_map.hpp>
#include <lax/channel/detail/use_lock.hpp>
#include <algorithm>

namespace lax
{
namespace channel
{

sub_map::sub_map(bool use_lock)
	: seq_(1, UINT32_MAX, 1000)
	, use_lock_(use_lock)
{
}

sub_map::~sub_map()
{

}

sub::key_t sub_map::subscribe(message::topic_t topic, sub::cond_t cond, sub::cb_t cb, sub::mode mode)
{
	use_lock_unique lock(use_lock_, mutex_);

	if (mode == sub::mode::immediate)
	{
		return subscribe(
			entries_immediate_,
			topic,
			cond,
			cb,
			mode);
	}
	else
	{
		return subscribe(
			entries_delayed_,
			topic,
			cond,
			cb,
			mode);
	}
}

sub::key_t sub_map::subscribe(message::topic_t topic, sub::cb_t cb, sub::mode mode)
{
	return subscribe(
		topic,
		[](message::ptr m) { return true; },
		cb,
		mode
	);
}

void sub_map::unsubscribe(sub::key_t key)
{
	use_lock_unique lock(use_lock_, mutex_);

	auto iter = keys_.find(key);
	check(iter != keys_.end());
	return_if(iter == keys_.end());

	if (iter->second.mode == sub::mode::immediate)
	{
		unsubscribe(entries_immediate_, key);
	}
	else
	{
		unsubscribe(entries_delayed_, key);
	}
}

std::size_t sub_map::post(message::topic_t topic, message::ptr m, sub::mode mode)
{
	use_lock_shared lock(use_lock_, mutex_);

	if (mode == sub::mode::immediate)
	{
		return post(entries_immediate_, topic, m);
	}
	else
	{
		return post(entries_delayed_, topic, m);
	}
}

std::size_t sub_map::post(message::ptr m, sub::mode mode)
{
	use_lock_shared lock(use_lock_, mutex_);

	if (mode == sub::mode::immediate)
	{
		return post(entries_immediate_, m);
	}
	else
	{
		return post(entries_delayed_, m);
	}
}

std::size_t sub_map::get_subscription_count(message::topic_t topic) const
{
	use_lock_shared lock(use_lock_, mutex_);

	int count = 0; 
	
	count += get_subscription_count(topic, sub::mode::immediate);
	count += get_subscription_count(topic, sub::mode::delayed);

	return count;
}

std::size_t sub_map::get_subscription_count(message::topic_t topic, sub::mode mode) const
{
	use_lock_shared lock(use_lock_, mutex_);

	const entry_map* target = &entries_immediate_;

	if (mode == sub::mode::delayed)
	{
		target = &entries_delayed_;
	}

	auto iter = target->find(topic);
	return_if(iter == target->end(), 0);

	return iter->second.subs.size();
}

sub::key_t sub_map::subscribe(
	entry_map& em,
	message::topic_t topic,
	sub::cond_t cond,
	sub::cb_t cb,
	sub::mode mode)
{
	auto iter = em.find(topic);

	if (iter == em.end())
	{
		entry e;

		auto key = subscribe(e, topic, cond, cb, mode);

		em.insert(entry_map::value_type(topic, e));

		return key;
	}
	// else

	return subscribe(iter->second, topic, cond, cb, mode);
}

sub::key_t sub_map::subscribe(
	entry& e,
	message::topic_t topic,
	sub::cond_t cond,
	sub::cb_t cb,
	sub::mode mode
)
{
	auto key = seq_.next();

	e.subs.push_back(
		sub(key, topic, cond, cb, mode)
	);

	auto ki = keys_.find(key);
	check(ki == keys_.end());

	keys_[key] = entry_link{ topic, mode };

	return key;
}

sub::key_t sub_map::unsubscribe(entry_map& em, sub::key_t key)
{
	auto iter = keys_.find(key);
	check(iter != keys_.end());
	return_if(iter == keys_.end());

	auto topic = iter->second;

	keys_.erase(iter);
	seq_.release(key);

	auto ei = em.find(key);
	check(ei != em.end());
	return_if(ei == em.end());

	auto& subs = ei->second.subs;

	subs.erase(
		std::remove_if(subs.begin(), subs.end(),
			[key](const sub& s) { return s.get_key() == key;}),
		subs.end()
	);
}

std::size_t sub_map::post(entry_map& em, message::topic_t topic, message::ptr m)
{
	auto iter = em.find(topic);
	return_if(iter == em.end(), 0);

	int count = 0;

	auto& subs = iter->second.subs;

	for (auto& sub : subs)
	{
		if (sub.post(m))
		{
			++count;
		}
	}

	return count + post(em, m);
}

std::size_t sub_map::post(entry_map& em, message::ptr m)
{
	auto iter = em.find(m->get_topic());
	return_if(iter == em.end(), 0);

	int count = 0;

	auto& subs = iter->second.subs;

	for (auto& sub : subs)
	{
		if (sub.post(m))
		{
			++count;
		}
	}

	return count;
}

} // channel
} // lax
