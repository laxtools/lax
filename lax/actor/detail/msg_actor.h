#pragma once 

#include <lax/actor/msg_keys.h>
#include <lax/actor/detail/ref.h>
#include <lax/net/protocol/msgpack/msg_pack.h>
#include <lax/net/protocol/json/msg_json.h>

#include <string>

namespace lax
{
namespace actor
{

struct msg_pack_actor : public net::msg_pack
{
	id src; 
	id dst;

	MSGPACK_BODY_OVERRIDE(msg_pack_actor);
	MSGPACK_DEFINE(src, dst);
};

struct msg_json_actor : public net::msg_pack
{
	// dst/src
	// runtime 다형성 추가
};

} // actor
} // lax
