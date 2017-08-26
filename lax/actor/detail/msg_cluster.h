#pragma once 

#include <lax/actor/detail/msg_addr_base.h>
#include <lax/actor/msg_keys.h>

#include <string>

namespace lax
{
namespace actor
{

struct msg_actor_state : public msg_addr_base
{
	id			id_;
	ref::role	role_;
	uint8_t		state_;
	std::string json_;
	
	MSGPACK_KEY(msg_types::type_cluster, msg_seqs::seq_cluster_update_state);
	MSGPACK_BODY_OVERRIDE(msg_actor_state);
	MSGPACK_DEFINE(id_, role_, state_, json_);
};



} // actor
} // lax
