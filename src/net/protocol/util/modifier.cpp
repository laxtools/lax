#include "stdafx.h"
#include <lax/net/protocol/util/modifier.hpp>

namespace lax
{
namespace net
{

void modifier::update_length_field(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t new_len
)
{
	auto iter = buf.begin() + (ptrdiff_t)msg_pos;
	iter; *iter = new_len & 0x000000FF;
	++iter; *iter = new_len >> 8 & 0x000000FF;
	++iter; *iter = new_len >> 8 & 0x000000FF;
	++iter; *iter = new_len >> 8 & 0x000000FF;
}

} // net
} // lax