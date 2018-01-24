#include "stdafx.h"
#include <lax/net/protocol/util/cipher.hpp>

namespace lax
{
namespace net
{

modifier::result cipher::on_bind()
{
	return result(false, reason::fail_not_implemented);
}

modifier::result cipher::on_recv(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	return result(false, reason::fail_not_implemented);
}

modifier::result cipher::on_send(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	return result(false, reason::fail_not_implemented);
}

} // net
} // lax