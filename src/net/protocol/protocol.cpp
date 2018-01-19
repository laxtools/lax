#include "stdafx.h"
#include <lax/net/protocol/protocol.hpp>
#include <lax/net/session.hpp>
#include <spdlog/fmt/fmt.h>

namespace lax
{
namespace net
{

protocol::protocol()
	: desc_("0:invalid")
{
}

protocol::~protocol()
{
}

bool protocol::bind(wptr ss)
{
	expect(ss);
	return_if(!ss, false);

	check(!session_);
	return_if(session_, false);

	session_ = ss;

	desc_ = fmt::format(
		"protocol/{0}/{1}", 
		ss->get_id().get_index(), 
		ss->get_remote_addr()
	);

	return !!session_;
}

} // net
} // lax