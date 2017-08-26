#include "stdafx.h"

#include "msg_local.h"
#include <lax/util/logger.h>
#include <lax/net/protocol/protocol.h>

namespace lax
{
namespace net
{

msg_local::msg_local()
	: msg(protocol::local)
{
}

msg_local::~msg_local()
{
}

} // net
} // lax
