#include "stdafx.h"
#include <lax/net/protocol/bits/bits_protocol.hpp>

namespace lax
{
namespace net
{

/// static configuration info
bits_protocol::config bits_protocol::cfg;

bits_protocol::bits_protocol()
{
}

bits_protocol::~bits_protocol()
{
}

void bits_protocol::on_bind()
{
	ensure(get_session());
}

protocol::result bits_protocol::send(packet::ptr m)
{
	return result(false, reason::fail_not_implemented);
}

protocol::result bits_protocol::on_recv(uint8_t* bytes, std::size_t len)
{
	return result(false, reason::fail_not_implemented);
}

void bits_protocol::on_send(std::size_t len)
{
}

void bits_protocol::on_error(const asio::error_code& ec)
{
}

} // net
} // lax