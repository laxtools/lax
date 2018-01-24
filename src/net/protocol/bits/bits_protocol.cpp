#include "stdafx.h"
#include <lax/net/protocol/bits/bits_protocol.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include <lax/net/protocol/bits/bits_message_factory.hpp>
#include <lax/net/session.hpp>
#include <lax/util/exception.hpp>
#include <lax/util/logger.hpp>

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
	expect(m);
	return_if(!m, result(false, reason::fail_null_message_pointer));

	auto mp = std::static_pointer_cast<bits_message>(m);

	if (!mp)
	{
		util::log()->critical(
			"bits_protocol expects bits_message to send. topic: {}/{}", 	
			m->get_topic().get_group(), m->get_topic().get_type()
		);

		return result(false, reason::fail_invalid_bits_message);
	}

	resize_buffer buf;

	auto size = mp->pack(buf);

	return_if(
		size < bits_message::header_length, 
		result(false, reason::fail_bits_pack_error)
	);

	buf.rewind(size); // rewind required since pack() changes buffer size 

	auto iter = buf.begin();
	  iter; *iter = size & 0x000000FF;
	++iter; *iter = size >> 8 & 0x000000FF;
	++iter; *iter = size >> 8 & 0x000000FF;
	++iter; *iter = size >> 8 & 0x000000FF;

	return send_final(mp, buf.data(), size);
}

protocol::result bits_protocol::send(
	packet::ptr m, 
	const uint8_t* const data, 
	std::size_t len)
{
	expect(m);
	expect(data);
	expect(len > 0);

	return_if(!m, result(false, reason::fail_null_message_pointer));
	return_if(!data, result(false, reason::fail_null_pointer));
	return_if(
		len < bits_message::header_length, 
		result(false, reason::fail_invalid_message_header)
	);

	auto mp = std::static_pointer_cast<bits_message>(m);

	if (!mp)
	{
		util::log()->critical(
			"bits_protocol expects bits_message to send. topic: {}/{}",
			m->get_topic().get_group(), m->get_topic().get_type()
		);

		return result(false, reason::fail_invalid_bits_message);
	}

	return send_final(mp, data, len);
}

protocol::result bits_protocol::send_final(
	bits_message::ptr mp,
	resize_buffer& buf,
	std::size_t len
)
{
	if (!needs_to_modify(mp))
	{
		if (cfg.enable_loopback)
		{
			// 테스트 모드일 경우 
			return on_recv(buf.data(), len);
		}

		// else

		return protocol::send(buf.data(), len);
	}

	return send_modified(mp, buf, len);
}

protocol::result bits_protocol::send_modified(
	bits_message::ptr mp,
	resize_buffer& buf,
	std::size_t len
)
{
	return result(false, reason::fail_not_implemented);
}

protocol::result bits_protocol::send_final(
	bits_message::ptr mp, 
	const uint8_t* const data, 
	std::size_t len
)
{
	if (!needs_to_modify(mp))
	{
		if (cfg.enable_loopback)
		{
			// 테스트 모드일 경우 
			return on_recv(data, len);
		}

		// else

		return protocol::send(data, len);
	}

	resize_buffer buf; 

	buf.append(data, len);

	return send_modified(mp, buf, len);
}

protocol::result bits_protocol::on_recv(
	const uint8_t* const bytes, 
	std::size_t len)
{
	expect(bytes);
	expect(len > 0);
	return_if(!bytes, result(false, reason::fail_null_pointer));
	return_if(len == 0, result(false, reason::fail_zero_size_data));

	recv_buf_.append(bytes, len);

	auto iter = recv_buf_.begin();

	auto remained_len = recv_buf_.end() - iter;
	std::size_t processed_len = 0;

	while (remained_len > bits_message::header_length)
	{
		auto msg_len = get_length(iter);	// forward iter while getting length

		if (remained_len < msg_len)
		{
			break;
		}

		auto topic = get_topic(iter);		// forward iter while getting topic 

		iter -= bits_message::header_length; // rewind  

		auto msg_begin_iter = iter;

		// TODO: cipher, checksum

		auto mp = BITS_MSG_CREATE(topic);

		if (!mp)
		{
			auto tp = bits_message::topic_t(topic);

			util::log()->error(
				"bits_message for topic[{}:{}] not registered",
				tp.get_group(), tp.get_type()
			);

			// session will be closed

			return result(false, reason::fail_bits_message_not_registered);
		}

		auto res = mp->unpack(recv_buf_, iter, msg_len);

		if (!res)
		{
			auto tp = bits_message::topic_t(topic);

			util::log()->error(
				"bits_message w/ topic[{}:{}] unpack error. {}",
				tp.get_group(), tp.get_type(),
				static_cast<uint8_t>(mp->reader_error)
			);

			// session will be closed

			return result(false, reason::fail_bits_unpack_error);
		}

		// success

		session::post(mp);

		processed_len += msg_len;
		iter += msg_len;
		remained_len = recv_buf_.end() - iter;
	}

	recv_buf_.pop_front(processed_len);

	return result(true, reason::success);
}

void bits_protocol::on_send(std::size_t len)
{
	len; 
	
	// 특별히 할 일은 없다
}

void bits_protocol::on_error(const asio::error_code& ec)
{
	ec;

	// 특별히 할 일이 없다.
}

uint32_t bits_protocol::get_length(resize_buffer::iterator& ri) const
{
	uint32_t len = 0;

	len = *ri;				++ri;
	len |= (*ri << 8);		++ri;
	len |= (*ri << 16);		++ri;
	len |= (*ri << 24);		++ri;

	return len;
}

uint32_t bits_protocol::get_topic(resize_buffer::iterator& ri) const
{
	uint32_t topic = 0;

	topic = *ri;				++ri;
	topic |= (*ri << 8);		++ri;
	topic |= (*ri << 16);		++ri;
	topic |= (*ri << 24);		++ri;

	return topic;
}

protocol::result bits_protocol::call_recv_for_test(
	const uint8_t* const bytes, 
	std::size_t len
)
{
	return on_recv(bytes, len);
}

bool bits_protocol::needs_to_modify(bits_message::ptr m) const
{
	return
		(cfg.enable_cipher		&& m->enable_cipher) ||
		(cfg.enable_checksum	&& m->enable_checksum) ||
		(cfg.enable_sequence	&& m->enable_sequence);
}


} // net
} // lax
