#pragma once 

#include <lax/channel/message.hpp>
#include <lax/util/sequence.hpp>

#include <asio.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <vector>

namespace lax
{
namespace net
{

/// handles session_closed subscriptions
/** 
 * session id based fast posting to subscribers 
 * - ref is used to sub here
 * - when a session is closed, 
 *   - callback is called with session_closed message for all subscriptions
 *   - then, all subs are removed after dispatching
 * 
 * callback is called from io thread 
 * - put the message into a queue, then process in its own thread.
 * - locking, slow processing can affect io performance.
 */
class close_subs
{
public:
	using sid = uint32_t;
	using key_t = uint32_t;
	using cb_t = std::function<void(channel::message::ptr)>;

public:
	/// 持失切
	close_subs();

	/// 社瑚切
	~close_subs();

	/// subscribe to topic. locked with unique_lock
	key_t subscribe(sid id, cb_t cb);

	/// unsubscribe to topic. locked with unique_lock
	void unsubscribe(key_t key);

	/// post a topic and message topic. locked with shared_lock
	std::size_t post(sid id, const asio::error_code& ec = asio::error::fault);

private:
	struct entry
	{
		key_t key;
		cb_t cb;
	};

	using vec = std::vector<entry>;
	using map = std::unordered_map<sid, vec>;
	using index = std::unordered_map<key_t, sid>;

private:
	std::recursive_mutex		mutex_;
	map							subs_;
	index						index_;
	util::sequence<key_t>		key_seq_;
};

} // channel
} // lax