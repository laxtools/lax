#pragma once 
#include <lax/server/service_actor.hpp>
#include <lax/util/simple_timer.hpp>
#include <lax/channel/channel.hpp>
#include <lax/channel/dispatcher.hpp>

namespace lax
{
namespace server
{

class server;

/// keeps connections to peers.
/** 
 * peer_service keeps connection to other servers. 
 * - initial connect 
 * - retry on close
 * - server runs peer_service (not task_scheduler)
 */
class peer_service final : public service_actor
{
public: 
	using id_t = service_actor::id_t;

	struct peer
	{
		enum state
		{
			constructed,
			connecting,
			connected,
			disconnected,
		};

		std::string			remote_addr;
		std::string			local_addr;
		std::string			protocol;
		state				state = state::constructed;
		std::size_t			fail_count = 0;
		std::size_t			connect_count = 0;
		float				reconnect_interval = 1.0f;
		float				last_try_time = 0.f;
	};

public: 
	/// add peers form _config
	peer_service(server& server, const nlm::json& _config);

	~peer_service();

	/// add host to connect 
	void add_peer(
		const std::string& remote_addr, 
		const std::string& protocol, 
		float reconnect_interval 
	);

private: 
	static constexpr float check_interval = 1.0f;

	/// peer w/ remote addr as a key
	using peers = std::map<std::string, peer>;

	/// start 
	virtual bool on_start() override;

	/// post messages in channel
	virtual void on_execute() override;

	/// finish log
	virtual void on_finish() override;

	/// process messages
	void process_messages(); 

	/// process reconnect
	void process_reconnect();

	/// 연결 실패
	void on_connect_failed();

	/// filter peers based on addr? 
	void on_session_ready();

	/// for peers only
	void on_session_closed();

	/// push message to queue
	void push_queue(channel::message::ptr m)
	{
		mq_.push(m);
	}

	void add_peers();

private:
	channel::channel::mq_t mq_;		/// queue to put messages
	util::simple_timer timer_;		/// timer to check reconnect
	peers peers_;					/// peers added from server
	channel::dispatcher disp_;		/// local dispatcher
};

} // server
} // lax