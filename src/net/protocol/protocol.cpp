#include "stdafx.h"
#include <lax/net/protocol/protocol.hpp>
#include <lax/net/session.hpp>
#include <lax/util/logger.hpp>

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
		"protocol:{0}:{1}", 
		ss->get_id().get_index(), 
		ss->get_remote_addr()
	);

	return !!session_;
}

protocol::result protocol::send(const uint8_t* const data, std::size_t len)
{
	return_if(!get_session(), result( false, reason::fail_invalid_session ));

	return get_session()->send(data, len);
}

void protocol::close()
{
	util::log()->info("close from protocol. {}", get_session()->get_desc());

	get_session()->close();
}

} // net
} // lax