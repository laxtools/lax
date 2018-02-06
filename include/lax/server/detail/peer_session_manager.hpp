#pragma once 
#include <lax/server/service_actor.hpp>

namespace lax
{
namespace server
{

class server;

/// peer_manager keeps connections to peers.
class peer_session_manager final
{
public: 
	using id_t = service_actor::id_t;

	struct peer
	{
		enum state
		{
			init,
			connecting,
			connected,
			up,
			down,
			disconnected,
		};

		std::string			remote_addr;
		bool				accepted = false;		// peer joined
		state				state = state::init;
		net::session::ref	sref;
	};

public: 
	peer_session_manager(server& server);

	~peer_session_manager();

	/// sub for connect failed topic
	bool start();

	void execute();

	void finish(); 

	/// add host to connect 
	void add_peer(const std::string& addr, const std::string& protocol);

private: 
	/// 연결 실패
	void on_connect_failed();

	/// filter peers based on addr? 
	void on_session_ready();

	/// for peers only
	void on_session_closed();

private:
	server& server_;
};

} // server
} // lax