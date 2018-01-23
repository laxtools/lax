#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include <lax/net/protocol/bits/bits_protocol.hpp>
#include <lax/net/protocol/bits/bits_message_factory.hpp>
#include <lax/util/botan/botan_all.h>

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

struct bm_flex : public bits_message
{
	BITS_MSG(
		3,				// group
		4,				// type
		bm_flex			// class 
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

} // noname

TEST_CASE("bits protocol")
{
	SECTION("send / recv")
	{
		bits_protocol::cfg.enable_loopback = true;

		BITS_MSG_REG(bm_flex);

		std::shared_ptr<bm_flex> res;

		auto skey = session::sub(
			bm_flex::topic_key,
			[&res](lax::channel::message::ptr m) {
				res = std::static_pointer_cast<bm_flex>(m);
			}
		);

		auto mp = std::make_shared<bm_flex>();

		mp->name = "Hello";
		mp->value = 1;
		mp->items.push_back(item{ 10000, 77777 });

		auto proto = std::make_shared<bits_protocol>();

		proto->send(mp); // post in on_recv

		REQUIRE(res->name == mp->name);

		session::unsub(skey);
	}

	SECTION("modifier")
	{
		SECTION("aes test")
		{
		
		}

		SECTION("checksum modifier")
		{

		}

		SECTION("cipher modifier")
		{

		}
	}

	SECTION("checksum")
	{
		bits_protocol::cfg.enable_loopback = true;


	}

	SECTION("cipher")
	{
		bits_protocol::cfg.enable_loopback = true;

	}


	SECTION("session")
	{
		bits_protocol::cfg.enable_loopback = false;


	}
}