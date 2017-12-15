#pragma once 

#include <lax/channel/sub.hpp>
#include <lax/util/sequence.hpp>
#include <map>
#include <shared_mutex>
#include <vector>

namespace lax
{
namespace channel
{

/// subscriptions map
/** 
 * used internally from channel class.
 *
 * thread-safe using shared_timed_mutex. 
 */
class sub_map
{
public: 
	/// 생성자
	sub_map(bool use_lock);

	/// 소멸자
	~sub_map();

	/// subscribe to topic. locked with unique_lock
	sub::key_t subscribe(
		message::topic_t topic, 
		sub::cond_t cond, 
		sub::cb_t cb, 
		sub::mode mode
	);

	/// subscribe to topic. locked with unique_lock
	sub::key_t subscribe(message::topic_t topic, sub::cb_t cb, sub::mode mode);

	/// unsubscribe to topic. locked with unique_lock
	bool unsubscribe(sub::key_t key);

	/// post a topic and message topic. locked with shared_lock
	std::size_t post(message::topic_t topic, message::ptr m, sub::mode mode);

	/// post a message topic. locked with shared_lock
	std::size_t post(message::ptr m, sub::mode mode);

	/// topic에 대해 나중 처리할 항목이 있는 지 확인
	bool has_delayed_sub(message::topic_t topic);

	/// 디버깅 용. topic에 등록된 개수
	std::size_t get_subscription_count(message::topic_t topic) const;

	/// 모드별 등록된 개수
	std::size_t get_subscription_count(message::topic_t topic, sub::mode mode) const;

private: 
	struct entry
	{
		message::topic_t	topic; 
		std::size_t			post_count = 0;
		std::vector<sub>	subs;
	};
	using entry_map = std::map<message::topic_t, entry>;

	struct entry_link
	{
		message::topic_t	topic;
		sub::mode			mode;
	};
	using key_map = std::map<sub::key_t, entry_link>;

	sub::key_t subscribe(
		entry_map& em, 
		message::topic_t topic, 
		sub::cond_t cond, 
		sub::cb_t cb, 
		sub::mode mode);

	sub::key_t subscribe(
		entry& e, 
		message::topic_t topic, 
		sub::cond_t cond, 
		sub::cb_t cb, 
		sub::mode mode
	);

	bool unsubscribe(entry_map& em, sub::key_t key);

	std::size_t post(entry_map& em, message::topic_t topic, message::ptr m);

	std::size_t post(entry_map& em, message::ptr m);

private:
	mutable std::shared_timed_mutex		mutex_;
	bool								use_lock_;
	entry_map							entries_immediate_;
	entry_map							entries_delayed_;
	key_map								keys_;
	util::sequence<sub::key_t>			seq_;
};

} // channel
} // lax