#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>

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

	std::string name;
	int value = 33;
	std::vector<item> items;

	BITS_SERIALIZE(
		name,
		value,
		items
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
	SECTION("message inheritance")
	{
		// pack / unpack message test

		//
	}
}