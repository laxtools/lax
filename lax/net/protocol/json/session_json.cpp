#include "stdafx.h"
#include <lax/net/protocol/json/session_json.h>
#include <lax/net/service.h>
#include <lax/util/logger.h>

namespace lax
{
namespace net
{
session_json::session_json(
	service& svc,
	const id& id,
	tcp::socket&& soc,
	bool accepted)
	: session(svc, id, std::move(soc), accepted)
{
	// don't use get_svc(). deadlock 가능
}

session_json::~session_json()
{
}

session::result session_json::send(msg::ptr m)
{
	auto jmp = std::static_pointer_cast<msg_json>(m);

	check(jmp.get());
	check(jmp->is_valid());

	const auto& str = jmp->get_str();

	check(str.length() >= 2);

	// check trailing \r\n
	{
		auto iter = str.rbegin();
		auto bn = *iter++;
		auto rn = *iter;

		check(bn == '\n');
		check(rn == '\r');
	}

	// since buf can be modified during send, we need to copy from str
	std::array<char, msg_json::max_string_length> buf;
	::memcpy(buf.data(), str.c_str(), str.length());

	++send_count_;

	return session::send((uint8_t*)buf.data(), str.length());
}

session::result session_json::test_recv(const uint8_t* data, std::size_t len)
{
	return on_recv(data, len);
}

void session_json::on_created()
{
	get_svc().get_app()->on_ready(get_id());
}

session::result session_json::on_recv(const uint8_t* data, std::size_t len)
{
	if ((buf_.size() + len) > buf_.capacity())
	{
		// xxx: must be configurable. exception or error
		return result(false, reason::fail_session_buffer_overflow);
	}

	buf_.append(data, len);

	parse_json();
	
	return result(true, reason::success);
}

session::result session_json::on_send(uint8_t* data, std::size_t len)
{
	return result(true, reason::success);
}

void session_json::on_error(const asio::error_code& ec)
{
	ec_ = ec;

	get_svc().get_app()->on_error(get_id(), ec);
}

void session_json::parse_json()
{
	enum state 
	{
		init, 
		found_r, 
		found_n
	};

	auto p = buf_.get();

	check(last_parsed_pos_ < p.size());

	std::size_t jbegin = 0;
	auto jend = jbegin;
	auto pos = last_parsed_pos_;

	std::size_t used_count = 0;

	auto st = state::init;


	while (pos < p.size())
	{
		auto ch = p.data()[pos];

		switch( ch )
		{
		case '\r':
			st = state::found_r;
			break;
		case '\n':
			if (st == state::found_r)
			{
				st = state::found_n;
				jend = pos;

				check(jend >= (jbegin + 1)); 

				used_count += build_msg(jbegin, jend);

				st = state::init;
				jbegin = pos + 1;		// n 다음 위치에서 시작
				jend = jbegin;
			}
			break;
		default: 
			st = state::init;
			break;
		}

		pos++;
	}

	check(pos >= used_count);

	buf_.pop_front(used_count);

	auto np = buf_.get();
	check(np.size() == p.size() - used_count);

	last_parsed_pos_ = pos - used_count;
}

std::size_t session_json::build_msg(std::size_t jbegin, std::size_t jend)
{
	// jend 는 \n

	auto p = buf_.get();

	check(p.size() > jbegin);
	check(p.size() > jend);
	check(jend >= (jbegin + 1)); // \r\n

	std::string m; 

	m.append((const char*)p.data(), jend - jbegin - 1); // ignore \r\n

	auto mp = std::make_shared<msg_json>();
	auto rc = mp->unpack(m);

	if (rc)
	{
		++recv_count_;

		get_svc().get_app()->on_recv(get_id(), mp);

		last_msg_ = mp;
	}
	else
	{
		// xxx: should be configurable. exception? 
		util::log()->info(
			"json message build failed. id: {0}, string: {1}", 
			get_id().get_value(), 
			m 
		);
	}

	return jend - jbegin + 1; // return used count
}

} // net
} // lax
