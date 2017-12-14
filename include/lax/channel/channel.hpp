#pragma once 

#include <lax/channel/message.hpp>
#include <lax/channel/sub.hpp>
#include <lax/channel/detail/sub_map.hpp>
#include <lax/util/concurrent_queue.hpp>
#include <atomic>
#include <functional>

namespace lax
{
namespace channel
{

class channel
{
public:
	using ptr = std::shared_ptr<channel>;
	using key_t = sub::channel_key_t;
	using cond_t = sub::cond_t;
	using cb_t = sub::cb_t;

	/// config 
	struct config
	{
		std::size_t loop_post_limit = 2048;			// limit post count in a post loop
		float time_to_log_when_over = 0.02;			// 20 ms
		float time_to_log_when_over = 0.2;			// 200 ms
		bool use_lock = true;						// use shared lock? 
		bool log_no_sub_when_post = false;
		bool post_all_when_destroyed = true;
	};

	/// thread-unsafe stat block
	struct stat
	{
		std::size_t post_time_over_count = 0;
		std::size_t post_loop_time_over_count = 0;

		std::size_t total_post_count = 0;
		std::size_t total_immediate_post_count = 0;
		std::size_t last_post_count = 0;
	};

public:
	/// create a channel and register it to a map
	/** 
	 * when a channel with a same key exists, it is returned 
	 */
	static ptr create(const key_t& key, const config& cfg);

	/// find a channel with a key
	static ptr find(const key_t& key);

	/// destroy a channel
	static void destroy(const key_t& key);

public:
	channel(const key_t& key, const config& cfg);

	~channel();

	/// push 
	/**
	 * @param m - message
	 *
	 * @return number of dispatched count
	 */
	std::size_t push(message::ptr m);

	/// push with a separate topic from message's topic
	/**
	 * @param topic - topic to dispatch first
	 * @param m - message
	 *
	 * dispatch with a topic first. then, call push(m) inside.
	 *
	 * @return number of dispatched cont
	 */
	std::size_t push(message::topic_t topic, message::ptr m);

	/// subscribe to a topic with a condition 
	/**
	 * @param topic - a topic to subscribe
	 * @param cond - condition to check before dispatch
	 * @param cb - callback function
	 * @param mode - dispatch mode
	 *
	 * @return the key to the subscription
	 */
	sub::key_t subscribe(
		message::topic_t topic, 
		cond_t cond, 
		cb_t cb, 
		sub::mode mode = sub::mode::delayed
	);

	/// subscribe to a topic without condition 
	/**
	 * @param topic - a topic to subscribe
	 * @param cb - callback function
	 *
	 * @return the key to the subscription
	 */
	sub::key_t subscribe(
		message::topic_t topic, 
		cb_t cb, 
		sub::mode mode = sub::mode::delayed
	);

	/// unsubscribe 
	/**
	 * @param key - a subscription key
	 */
	void unsubscribe(sub::key_t key);

	/// post all messages in queue
	/**
	 * sub::mode::delayed subscriptions are handled for messages
	 */
	std::size_t post();

	/// queue size (unsafe)
	std::size_t get_queue_size() const;

	/// stat
	const stat& get_stat() const
	{
		return stat_;
	}

private:
	using mq = util::concurrent_queue<message::ptr>;

private:
	key_t   key_;
	mq		q_;
	sub_map map_;
	config  config_;
	stat	stat_;
};

} // channel
} // lax