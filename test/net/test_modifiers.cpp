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

			auto rc2 = seq.on_recv(buf, 0, buf.size());
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
				REQUIRE(buf.data()[buf.size() - 1] == ((i+1) % 256));

				auto rc2 = seq.on_recv(buf, 0, buf.size());
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
		REQUIRE(old_size + 4 == buf.size());

		auto rc2 = csum.on_recv(buf, 0, buf.size());
		REQUIRE(!!rc2);
	}

	SECTION("cipher")
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

		auto old_size = buf.size();
		auto rc = enc.on_send(buf, 0, buf.size());

		REQUIRE(!!rc);
		REQUIRE(old_size == buf.size());

		auto rc2 = enc.on_recv(buf, 0, buf.size());
		REQUIRE(!!rc2);
	}
}