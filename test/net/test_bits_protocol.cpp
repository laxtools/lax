#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>

using namespace lax::net;

// to make this test readable
#include "bits_message_design.hpp"

namespace
{

struct bm_derived : public bits_message
{
	BITS_MSG_TOPIC(3, 3);		// group, type
	BITS_MSG_CLASS(bm_derived); // constructor and pack/unpack

	std::string name;
};

// must be in global, anonymous or bitsery namespace
template <typename S>
void serialize(S& ss, bm_derived& m)
{
	BITS_SERIALIZE_TOPIC(ss, m);

	ss.text1b(m.name, 64);
}

} // noname

TEST_CASE("bits protocol")
{
	SECTION("bits message")
	{
		auto mp = std::make_shared<bm_derived>();
		mp->name = "Hello";

		resize_buffer buf;

		auto size = mp->pack(buf);
		REQUIRE(size > 0);

		auto res = std::make_shared<bm_derived>();
		auto rc = res->unpack(buf, buf.begin(), size);

		REQUIRE(res->name == mp->name);
	}

	SECTION("communication w/ lambda callbacks")
	{
		auto& svc = service::inst();

		svc.init();



		svc.fini();
	}

	SECTION("performance")
	{
		// single connection send / recv 
		// aims for 1G bps, 1M pps
	}

	SECTION("defining messages")
	{
		// design test
		// - implementation of pack/unpack
		// - recv message packetization
		// 

		SECTION("pack / unpack. 1st try")
		{
			auto mp = std::make_shared<bm>();
			mp->v = 3;

			resize_buffer buf;

			auto size = mp->pack(buf);
			REQUIRE(size > 0);

			auto res = std::make_shared<bm>();
			auto rc = res->unpack(buf.begin(), size);

			REQUIRE(res->v == 3);
		}

		SECTION("pack / unpack. 2nd try")
		{
			auto mp = std::make_shared<bm2>();
			mp->name = "Hello";

			resize_buffer buf;

			auto size = mp->pack(buf);
			REQUIRE(size > 0);

			auto res = std::make_shared<bm2>();
			auto rc = res->unpack(buf, buf.begin(), size);

			REQUIRE(res->name == mp->name);
		}
	}
}