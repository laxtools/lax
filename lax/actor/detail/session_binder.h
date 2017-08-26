#pragma once 

#include <lax/actor/id.h>
#include <lax/actor/detail/index.h>
#include <lax/actor/detail/multi_index.h>
#include <lax/actor/reason.h>
#include <lax/net/session.h>

namespace lax
{
namespace actor
{

class actor;

/// per actor session bookkeeping
/** 
 * 
 */
class session_binder
{
public: 
	using result = util::result<bool, reason>;
	using sid_vec = std::vector<net::session::id>;
	using aid_vec = std::vector<id>;

	session_binder(); 
	~session_binder();

	/// bind a session to send to the session directory
	result bind(net::session::ptr ptr);

	/// bind a session with an actor id to send to the session with an actor id
	result bind(id aid, net::session::ptr ptr);

	/// send to a session
	result send(net::session::id sid, net::msg::ptr m);

	/// send to an actor
	result send(id aid, net::msg::ptr m);

	/// send to actors. return send count
	int send(const aid_vec& aids, net::msg::ptr m);

	/// send to sessions. returns send count
	int send(const sid_vec& sids, net::msg::ptr m);

	/// session id binding 체크
	bool has(net::session::id sid);

	/// actor id 바인딩 체크
	bool has(id aid);

	/// unbind session
	void unbind(net::session::id sid);

	/// unbind actor
	void unbind(id aid);

	/// 각 프로토콜 해석해서 버퍼에 넣음
	result send_proto(net::session::ptr ss, net::msg::ptr m);

private: 
	using session_index = index<net::session::id, net::session::ptr>;
	using actor_index = index<id, net::session::ptr>;
	using buffer = msgpack::sbuffer;

private:
	session_index	session_index_;
	actor_index		actor_index_;
};

} // actor 
} // lax
