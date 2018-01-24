#include "stdafx.h"
#include <lax/net/protocol/util/checksum.hpp>
#include <lax/util/botan/botan_all.h>

namespace lax
{
namespace net
{

modifier::result checksum::on_bind()
{
	return result(true, reason::success);
}

modifier::result checksum::on_recv(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	expect(msg_len > 0);
	expect(buf.size() >= msg_pos + msg_len - 1);

	// for thread-safey, create hash function 
	// android NDK supports it with clang
	// 
	static thread_local auto hash = Botan::HashFunction::create("CRC32");
	return_if(!hash, result(false, reason::fail_null_hash_function));

	hash->update(buf.data() + msg_pos, msg_len - checksum_size);

	uint8_t crc[checksum_size];
	hash->final(crc);

	auto rc = std::memcmp(
		buf.data() + msg_pos + msg_len- checksum_size, 
		crc, 
		checksum_size
	);

	if (rc != 0)
	{
		return result(false, reason::fail_incorrect_checksum);
	}

	return result(true, reason::success);
}

modifier::result checksum::on_send(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	expect(msg_len > 0);
	expect(buf.size() >= msg_pos + msg_len - 1);

	// for thread-safey, create hash function 
	static thread_local auto hash = Botan::HashFunction::create("CRC32");
	return_if(!hash, result(false, reason::fail_null_hash_function));

	hash->update(buf.data() + msg_pos, msg_len);

	uint8_t crc[checksum_size];
	hash->final(crc);

	buf.append("abcd", checksum_size); // resize 

	std::memcpy(buf.data() + msg_pos + msg_len, crc, checksum_size);

	return result(true, reason::success);
}

} // net
} // lax