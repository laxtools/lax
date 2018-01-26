#include "stdafx.h"
#include <lax/net/protocol/util/checksum.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
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
	std::size_t msg_len,
	std::size_t& new_len
)
{
	new_len = msg_len;

	expect(msg_len >= bits_message::header_length);

	// payload 없으면 성공으로 처리
	return_if(
		msg_len == bits_message::header_length , 
		result(true, reason::success)
	);

	// for thread-safey, create hash function 
	// - windows, android, ios, osx, linux supports it
	static thread_local auto hash = Botan::HashFunction::create("CRC32");
	return_if(!hash, result(false, reason::fail_null_hash_function));

	hash->clear();

	hash->update(
		buf.data() + msg_pos + bits_message::header_length, 
		msg_len - checksum_size - bits_message::header_length
	);

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

	new_len = msg_len - checksum_size;

	return result(true, reason::success);
}

modifier::result checksum::on_send(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	expect(msg_len >= bits_message::header_length);

	// payload 없으면 성공으로 처리
	return_if(
		msg_len == bits_message::header_length, 
		result(true, reason::success)
	);

	// for thread-safey, create hash function 
	static thread_local auto hash = Botan::HashFunction::create("CRC32");
	return_if(!hash, result(false, reason::fail_null_hash_function));

	hash->clear();

	hash->update(
		buf.data() + msg_pos + bits_message::header_length, 
		msg_len - bits_message::header_length
	);

	uint8_t crc[checksum_size];
	hash->final(crc);

	buf.append("abcd", checksum_size); // 공간 확보

	std::memcpy(buf.data() + msg_pos + msg_len, crc, checksum_size);

	update_length_field(buf, msg_pos, msg_len + checksum_size);

	return result(true, reason::success);
}

} // net
} // lax