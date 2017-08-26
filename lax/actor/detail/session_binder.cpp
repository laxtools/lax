#include "stdafx.h"
#include "session_binder.h"
#include <lax/net/protocol/protocol.h>
#include <lax/net/protocol/msgpack/session_pack.h>
#include <lax/net/protocol/local/msg_local.h>
#include <lax/net/protocol/json/session_json.h>

namespace lax
{
namespace actor
{

session_binder::session_binder()
{
}

session_binder::~session_binder()
{
	// all cleared auto... RAII! 
}

session_binder::result
session_binder::bind(net::session::ptr ptr)
{
	check(ptr);
	return_if(!ptr, result(false, reason::fail_null_pointer));

	auto rc1 = session_index_.find(ptr->get_id());

	return_if( rc1, result(false, reason::faill_session_already_bound) );

	auto rc2 = session_index_.link(ptr->get_id(), ptr);

	return_if(!rc2, result(false, reason::fail_session_bind));

	return result(true, reason::success);
}

session_binder::result
session_binder::bind(id aid, net::session::ptr ptr)
{
	check(aid.is_valid());
	return_if(!aid.is_valid(), result(false, reason::fail_invalid_actor_id));

	check(ptr);
	return_if(!ptr, result(false, reason::fail_null_pointer));

	auto rc = actor_index_.link(aid, ptr);
	return_if(!rc, result(false, reason::fail_actor_session_bind));

	return result(true, reason::success);
}

session_binder::result 
session_binder::send(net::session::id sid, net::msg::ptr m)
{
	auto rc = session_index_.find(sid);

	return_if(!rc, result(false, reason::fail_session_not_found));

	return_if(!rc.value->is_open(), result(false, reason::fail_session_closed));

	return send_proto(rc.value, m);
}

session_binder::result 
session_binder::send(id aid, net::msg::ptr m)
{	
	auto rc = actor_index_.find(aid);
	return_if(!rc, result(false, reason::fail_no_bound_actor));

	return_if(!rc.value->is_open(), result(false, reason::fail_session_closed));
	
	return send_proto(rc.value, m);
}

int session_binder::send(const aid_vec& aids, net::msg::ptr m)
{
	int send_count = 0;

	for (auto& aid : aids)
	{
		auto rc = send(aid, m);

		if (rc) send_count++;
	}

	return send_count;
}

int session_binder::send(const sid_vec& sids, net::msg::ptr m)
{
	int send_count = 0;

	for (auto& sid : sids)
	{
		auto rc = send(sid, m);

		if (rc) send_count++;
	}

	return send_count;
}

bool session_binder::has(net::session::id sid)
{
	return session_index_.has(sid);
}

bool session_binder::has(id aid)
{
	return actor_index_.has(aid);
}

void session_binder::unbind(net::session::id sid)
{
	session_index_.unlink(sid);
}

void session_binder::unbind(id aid)
{
	actor_index_.unlink(aid);
}

session_binder::result 
session_binder::send_proto(net::session::ptr ss, net::msg::ptr m)
{
	net::session::result rc;

	switch (m->get_protocol())
	{
	case net::protocol::msgpack:
	{
		auto ss_pack = std::static_pointer_cast<net::session_pack>(ss);
		rc = ss_pack->send(m);
	}
	break;
	case net::protocol::json:
	{
		auto ss_json = std::static_pointer_cast<net::session_json>(ss);
		rc = ss_json->send(m);
	}
	break;
	default: 
	{
		// unsupported network protocol	
	}
	break;
	}

	return_if(!rc, result(false, reason::fail_send_to_session));

	return result(true, success);
}

} // actor
} // lax
