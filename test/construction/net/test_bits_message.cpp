#include "stdafx.h"
#include <lax/net/protocol/bits/bits_message.hpp>
#include <catch.hpp>

using namespace lax::net;

namespace
{

struct item
{
	int id = 1;
	int cost = 100;

	BITS_SERIALIZE(
		id,
		cost
	);
};

struct bm_flex_base : public bits_message
{
	BITS_MSG_BASE(
		bm_flex_base	// class 
	);

	int key = 3;

	BITS_SERIALIZE(
		key
	);
}; 

struct bm_flex_inh : public bm_flex_base
{
	BITS_MSG_INH(
		3,				// group
		5,				// type
		bm_flex_inh,	// class 
		bm_flex_base	// base
	);

	std::string name;
	int value = 33;
	std::vector<item> items;

	BITS_SERIALIZE_INH(
		name,
		value,
		items
	);
};

} // noname

TEST_CASE("test bits message")
{
	// pack / unpack test

	SECTION("message inheritance")
	{
		// pack / unpack message test
		resize_buffer buf; 

		auto mp = std::make_shared<bm_flex_inh>();

		mp->key = 55;

		auto size = mp->pack(buf);
		REQUIRE(size > 0);

		auto res = std::make_shared<bm_flex_inh>();
		auto rc = res->unpack(buf, buf.begin(), size);

		REQUIRE(rc);
		REQUIRE(res->key == 55);
	}
}