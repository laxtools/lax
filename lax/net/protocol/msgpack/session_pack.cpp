#include "stdafx.h"

#include <lax/net/protocol/msgpack/session_pack.h>
#include <lax/net/protocol/msgpack/msg_pack_factory.h>
#include <lax/net/service.h>
#include <lax/util/logger.h>

namespace lax
{
namespace net
{
session_pack::session_pack(
	service& svc,
	const id& id,
	tcp::socket&& soc,
	bool accepted)
	: session(svc, id, std::move(soc), accepted)
{
	// don't use get_svc(). deadlock 가능
}

session_pack::~session_pack()
{
}

session::result session_pack::send(msg::ptr m)
{
	return_if(
		!m, result(false, reason::fail_null_pointer);
	)

	buffer buf;		// TODO : 버퍼의 효율적인 관리는? 

	auto mp = std::static_pointer_cast<msg_pack>(m);

	// 길이 필드 확보
	uint32_t len = 0;
	uint32_t key = mp->get_key();

	buf.write((const char*)&len, sizeof(len));
	buf.write((const char*)&key, sizeof(key));

	try
	{
		mp->pack(buf);
	}
	catch (std::exception& ex)
	{
		util::log()->error(
			"sesson_pack. pack error. id: {0}. key: {1} exception: {2}",
			get_id().get_value(),
			mp->get_key(), 
			ex.what()
		);

		return result(false, reason::fail_msg_pack_serialize);
	}

	check(buf.size() < UINT32_MAX);
	check(buf.size() >= header_length);
	
	len = static_cast<uint32_t>(buf.size()); // 0 byte도 지원하기 위해 len는 자기 필드를 포함

	// 길이 쓰기. shift is endian-safe. 거꾸로 씀
	char* dp = buf.data();

	dp[3] = len & 0xff;
	dp[2] = (len >> 8) & 0xff;
	dp[1] = (len >> 16) & 0xff;
	dp[0] = (len >> 24) & 0xff;

	dp[7] = key & 0xff;
	dp[6] = (key >> 8) & 0xff;
	dp[5] = (key >> 16) & 0xff;
	dp[4] = (key >> 24) & 0xff;

	send_count_++;

	return session::send((uint8_t*)buf.data(), buf.size());
}

session::result session_pack::test_recv( const uint8_t* data, std::size_t len)
{
	return on_recv(data, len);
}

void session_pack::on_created()
{
	get_svc().get_app()->on_ready(get_id());
}

session::result session_pack::on_recv(const uint8_t* data, std::size_t len)
{
	buf_.write((const char*)data, len);

	parse_msgs();

	return result(true, reason::success);
}

session::result session_pack::on_send(uint8_t* data, std::size_t len)
{
	return result(true, reason::success);
}

void session_pack::on_error(const asio::error_code& ec)
{
	ec_ = ec;

	get_svc().get_app()->on_error(get_id(), ec);
}

void session_pack::parse_msgs()
{
	const uint8_t* dp = (const uint8_t*)buf_.data();
	std::size_t pos = 0; // 처리 위치
	std::size_t total_used = 0;

	while ( (pos + header_length) < buf_.size()) 
	{
		uint32_t len = 0;

		// 거꾸로 쓰여 있어 shift로 만듦
		len = len << 8 | dp[pos+0];
		len = len << 8 | dp[pos+1];
		len = len << 8 | dp[pos+2];
		len = len << 8 | dp[pos+3];

		if (buf_.size() < (pos + len))
		{
			break;
		}

		// TODO: configurea max_packet_length
		if (len > max_packet_length)
		{
			util::log()->error(
				"session_pack. invalid packet length. id: {0}. len: {1}", 
				get_id().get_value(), len
			);

			close();

			return;
		}

		// get key
		uint32_t key = 0;
		key = key << 8 | dp[pos+4];
		key = key << 8 | dp[pos+5];
		key = key << 8 | dp[pos+6];
		key = key << 8 | dp[pos+7];

		if ( key == 0 )
		{
			// invalid msg key 

			util::log()->error(
				"session_pack. invalid domain or type. id: {0}. len: {1}, key: {2}",
				get_id().get_value(), len, key
			);
		}
		else
		{
			auto msg = msg_pack_factory::instance().create(key);

			if (msg)
			{
				auto rc = msg->unpack((const char*)&dp[pos+header_length], len);

				if (rc)
				{
					check(msg->is_valid());

					msg->set_session(get_id().get_value());

					last_msg_ = msg;

					get_svc().get_app()->on_recv(get_id(), msg);

					recv_count_++;
				}
				else
				{
					// TODO: 구성 가능하게 해서 close 여부 결정

					util::log()->error(
						"session_pack. failed to unpack a message."
						"id: {0}. len: {1}, key: {2}, error: {3}",
						get_id().get_value(), len, key, rc.value
					);
				}
			}
			else
			{
				// TODO: 구성 가능하게 해서 close 여부 결정

				util::log()->error(
					"session_pack. failed to create a message."
					"id: {0}. len: {1}, key: {2}",
					get_id().get_value(), len, key
				);
			}
		}

		total_used += len;
		pos += len;
	}

	if (total_used > 0)
	{
		buf_.pop(total_used);
	}
}

} // net
} // lax
