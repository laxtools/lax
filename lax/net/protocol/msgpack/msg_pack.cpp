#include "stdafx.h"

#include "msg_pack.h"
#include <lax/util/logger.h>
#include <lax/net/protocol/protocol.h>

namespace lax
{
namespace net
{

msg_pack::msg_pack()
	: msg(protocol::msgpack)
{
}

msg_pack::~msg_pack()
{
}

} // net
} // lax
