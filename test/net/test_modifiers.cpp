#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/protocol/util/sequencer.hpp>
#include <lax/net/protocol/util/checksum.hpp>
#include <lax/net/protocol/util/cipher.hpp>
#include <lax/net/protocol/bits/bits_protocol.hpp>

using namespace lax::net;

namespace
{

struct msg_test : public bits_message
{
	BITS_MSG(1, 1, msg_test);

	int a; 
	std::string b;

	BITS_SERIALIZE(a, b);
};

}

TEST_CASE("modifiers test")
{
	bits_protocol::cfg.enable_loopback = true;

	SECTION("sequencer")
	{
		SECTION("simple send and recv")
		{
			sequencer seq;

			auto pp = std::make_shared<bits_protocol>();

			seq.bind(pp.get());

			resize_buffer buf;

			auto mp = std::make_shared<msg_test>();
			mp->a = 3;
			mp->b = "Hello";
			auto wsize = mp->pack(buf);

			// ����: pack()�� buf�� ũ��� �޼��� ũ��� ���� ó������ �ʴ´�. 
			// resize()���� ���̸� �� ���� Ȯ���Ѵ�. 
			// resize()�� �ϸ� pos_�� �ٲ��. 

			// send simulation

			// bitsery���� ���� ���۷� �����ϰ� ���� 
			// ������ ������ �־ pack �Ŀ� �����ϴ� ������ �Ѵ�. 
			buf.rewind(wsize);

			auto old_size = buf.size();
			auto rc = seq.on_send(buf, 0, buf.size());

			REQUIRE(!!rc);
			REQUIRE(old_size + 1 == buf.size());
			REQUIRE(buf.data()[buf.size() - 1] == 1);

			std::size_t new_len = 0;
			auto rc2 = seq.on_recv(buf, 0, buf.size(), new_len);
			REQUIRE(!!rc2);
		}

		SECTION("many send and recv")
		{
			sequencer seq;

			auto pp = std::make_shared<bits_protocol>();

			seq.bind(pp.get());

			// 256 �Ѿ �׽�Ʈ 
			for (int i = 0; i < 300; ++i)
			{
				resize_buffer buf;

				auto mp = std::make_shared<msg_test>();
				mp->a = 3;
				mp->b = "Hello";
				auto wsize = mp->pack(buf);
				buf.rewind(wsize);

				auto old_size = buf.size();
				auto rc = seq.on_send(buf, 0, buf.size());

				REQUIRE(!!rc);
				REQUIRE(old_size + 1 == buf.size());
				REQUIRE(buf.data()[buf.size() - 1] == ((i + 1) % 256));

				std::size_t new_len = 0;
				auto rc2 = seq.on_recv(buf, 0, buf.size(), new_len);
				REQUIRE(!!rc2);
			}
		}
	}

	SECTION("checksum")
	{
		checksum csum;

		auto pp = std::make_shared<bits_protocol>();

		csum.bind(pp.get());

		resize_buffer buf;

		auto mp = std::make_shared<msg_test>();
		mp->a = 3;
		mp->b = "Hello";

		auto wsize = mp->pack(buf);
		buf.rewind(wsize);

		auto old_size = buf.size();
		auto rc = csum.on_send(buf, 0, buf.size());

		REQUIRE(!!rc);

		auto new_size = buf.size();
		REQUIRE(old_size + 4 == new_size);

		std::size_t new_len = 0;
		auto rc2 = csum.on_recv(buf, 0, buf.size(), new_len);
		REQUIRE(!!rc2);
	}

	SECTION("cipher")
	{
		SECTION("cipher basic")
		{
			cipher enc;

			auto pp = std::make_shared<bits_protocol>();

			enc.bind(pp.get());

			resize_buffer buf;

			auto mp = std::make_shared<msg_test>();
			mp->a = 3;
			mp->b = "Hello";

			auto wsize = mp->pack(buf);
			buf.rewind(wsize);

			auto rc = enc.on_send(buf, 0, buf.size());
			REQUIRE(!!rc);

			std::size_t new_len = 0;
			auto rc2 = enc.on_recv(buf, 0, buf.size(), new_len);
			REQUIRE(!!rc2);

			auto res = std::make_shared<msg_test>();
			res->unpack(buf, buf.begin(), buf.size());

			REQUIRE(res->b == "Hello");
		}

		SECTION("cipher loop")
		{
			cipher::cfg.key_change_interval = 8;

			auto pp = std::make_shared<bits_protocol>();

			for (int i = 0; i < 32; ++i)
			{
				cipher enc;

				enc.bind(pp.get());

				resize_buffer buf;

				auto mp = std::make_shared<msg_test>();
				mp->a = 3;
				mp->b = "Hello Loop";

				auto wsize = mp->pack(buf);
				buf.rewind(wsize);

				auto rc = enc.on_send(buf, 0, buf.size());
				REQUIRE(!!rc);

				std::size_t new_len = 0;
				auto rc2 = enc.on_recv(buf, 0, buf.size(), new_len);
				REQUIRE(!!rc2);

				auto res = std::make_shared<msg_test>();
				res->unpack(buf, buf.begin(), buf.size());

				REQUIRE(res->b == "Hello Loop");
			}
		}

		SECTION("big message")
		{
			cipher enc;

			auto pp = std::make_shared<bits_protocol>();

			enc.bind(pp.get());

			resize_buffer buf;

			auto mp = std::make_shared<msg_test>();
			mp->a = 3;

			for (int i = 0; i < 100 ; ++i)
			{
				mp->b.append("12345678901234567890123456789012");
			}

			auto wsize = mp->pack(buf);
			buf.rewind(wsize);

			auto rc = enc.on_send(buf, 0, buf.size());
			REQUIRE(!!rc);

			std::size_t new_len = 0;
			auto rc2 = enc.on_recv(buf, 0, buf.size(), new_len);
			REQUIRE(!!rc2);

			auto res = std::make_shared<msg_test>();
			res->unpack(buf, buf.begin(), buf.size());

			REQUIRE(res);
		}
	}
}