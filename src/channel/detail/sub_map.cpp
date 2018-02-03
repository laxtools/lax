#include "stdafx.h"
#include <lax/channel/detail/sub_map.hpp>
#include <lax/channel/detail/use_lock.hpp>
#include <lax/util/logger.hpp>
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

sub::key_t sub_map::subscribe(const message::topic_t& topic, sub::cond_t cond, sub::cb_t cb, sub::mode mode)
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

sub::key_t sub_map::subscribe(const message::topic_t& topic, sub::cb_t cb, sub::mode mode)
{
	return subscribe(
		topic,
		[](message::ptr m) { return true; },
		cb,
		mode
	);
}

bool sub_map::unsubscribe(sub::key_t key)
{
	use_lock_unique lock(use_lock_, mutex_);

	auto iter = keys_.find(key);
	VERIFY(iter != keys_.end());
	RETURN_IF(iter == keys_.end(),false);

	if (iter->second.mode == sub::mode::immediate)
	{
		return unsubscribe(entries_immediate_, key);
	}
	else
	{
		return unsubscribe(entries_delayed_, key);
	}
}

std::size_t sub_map::post(const message::topic_t& topic, message::ptr m, sub::mode mode)
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

bool sub_map::has_delayed_sub(const message::topic_t& topic)
{
	use_lock_shared lock(use_lock_, mutex_);

	auto iter = entries_delayed_.find(topic);

	return iter != entries_delayed_.end();
}

std::size_t sub_map::get_subscription_count(const message::topic_t& topic) const
{
	use_lock_shared lock(use_lock_, mutex_);

	std::size_t count = 0; 
	
	count += get_subscription_count(topic, sub::mode::immediate);
	count += get_subscription_count(topic, sub::mode::delayed);

	return count;
}

std::size_t sub_map::get_subscription_count(const message::topic_t& topic, sub::mode mode) const
{
	use_lock_shared lock(use_lock_, mutex_);

	const entry_map* target = &entries_immediate_;

	if (mode == sub::mode::delayed)
	{
		target = &entries_delayed_;
	}

	auto iter = target->find(topic);
	RETURN_IF(iter == target->end(), 0);

	return iter->second.subs.size();
}

sub::key_t sub_map::subscribe(
	entry_map& em,
	const message::topic_t& topic,
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
	const message::topic_t& topic,
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
	VERIFY(ki == keys_.end());

	keys_[key] = entry_link{ topic, mode };

	return key;
}

bool sub_map::unsubscribe(entry_map& em, sub::key_t key)
{
	auto iter = keys_.find(key);
	VERIFY(iter != keys_.end());
	RETURN_IF(iter == keys_.end(), false);

	auto topic = iter->second.topic;

	keys_.erase(iter);
	seq_.release(key);

	auto ei = em.find(topic);
	VERIFY(ei != em.end());
	RETURN_IF(ei == em.end(), false);

	auto& subs = ei->second.subs;

	auto si = std::find_if(
		subs.begin(), subs.end(), 
		[key](const sub& s) {return s.get_key() == key;}
	);

	if ( si == subs.end())
	{
		return false;
	}

	// 등록할 때 동일 키는 추가되지 않도록 검증

	subs.erase(si);

	return true;
}

std::size_t sub_map::post(entry_map& em, const message::topic_t&topic, message::ptr m)
{
	std::size_t count = 0; 

	count += post_on_topic(em, topic, m);
	count += post_on_topic(em, topic.get_group_topic(), m);
	count += post_on_topic(em, m->get_topic(), m);
	count += post_on_topic(em, m->get_topic().get_group_topic(), m);

	return count;
}

std::size_t sub_map::post(entry_map& em, message::ptr m)
{
	std::size_t count = 0; 

	count += post_on_topic(em, m->get_topic(), m);
	count += post_on_topic(em, m->get_topic().get_group_topic(), m);

	return count;
}

std::size_t sub_map::post_on_topic(entry_map& em, const message::topic_t& topic, message::ptr m)
{
	// 테스트 용도 등으로 valid 하지 않은 경우 발생 
	RETURN_IF(!topic.is_valid(), 0);

	auto iter = em.find(topic);
	RETURN_IF(iter == em.end(), 0);

	int count = 0;

	auto& subs = iter->second.subs;

	for (auto& sub : subs)
	{
		if (sub.post(m))
		{
			++count;

			util::log()->debug(
				"post. msg: {0} topic: {1}/{2} count: {3}", 
				m->get_desc(),
				topic.get_group(), 
				topic.get_type(), count
			);
		}
	}

	return count;
}


} // channel
} // lax
