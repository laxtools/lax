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

struct bm_flex_cipher : public bits_message
{
	BITS_MSG_DETAIL(
		3,				// group
		5,				// type
		bm_flex_cipher,	// class 
		true,			// enc
		true,			// crc
		true			// seq
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
		SECTION("modifier loopback test")
		{
			bits_protocol::cfg.enable_loopback = true;

			BITS_MSG_REG(bm_flex_cipher);

			std::shared_ptr<bm_flex_cipher> res;

			auto skey = session::sub(
				bm_flex_cipher::topic_key,
				[&res](lax::channel::message::ptr m) {
				res = std::static_pointer_cast<bm_flex_cipher>(m);
			}
			);

			auto mp = std::make_shared<bm_flex_cipher>();

			mp->name = "Hello";
			mp->value = 1;
			mp->items.push_back(item{ 10000, 77777 });

			auto proto = std::make_shared<bits_protocol>();

			auto rc = proto->send(mp); // post in on_recv
			REQUIRE(rc);
			REQUIRE(res->name == mp->name);

			session::unsub(skey);
		}

		SECTION("modifier multiple test")
		{
			bits_protocol::cfg.enable_loopback = true;

			BITS_MSG_REG(bm_flex_cipher);

			std::shared_ptr<bm_flex_cipher> res;

			auto skey = session::sub(
				bm_flex_cipher::topic_key,
				[&res](lax::channel::message::ptr m) {
				res = std::static_pointer_cast<bm_flex_cipher>(m);
			}
			);

			auto mp = std::make_shared<bm_flex_cipher>();

			mp->name = "Hello";
			mp->value = 1;
			mp->items.push_back(item{ 10000, 77777 });

			auto proto = std::make_shared<bits_protocol>();

			for (int i = 0; i < 10000 ; ++i)
			{
				mp->value = i;
				auto rc = proto->send(mp); // post in on_recv
				REQUIRE(rc);
				REQUIRE(res->name == mp->name);
				REQUIRE(res->value == i);
			}

			//
			// 암호화 대략적인 성능
			// - seq / crc / enc 양방향 처리
			//   - 1백만 3초 걸림
			//   - 초당 30만 정도 (패킷 길이에 따라 다름) 
			// - AES128 
			//   - 10 iterations
			//   - not using SSE3 
			// - 암호화는 락을 거는 것이 더 문제 
			//   - 서버에서 플레이 검증 하는 것이 중요
			// 

			session::unsub(skey);
		}

		SECTION("modifier network test")
		{
			bits_protocol::cfg.enable_loopback = false;

			auto& svc = service::inst();

			svc.init();

			svc.listen("127.0.0.1:7777", "bits");

			svc.connect("127.0.0.1:7777", "bits");

			svc.wait(1000);

			REQUIRE(svc.get_session_count() == 2);

			// send 

			// check recv

			svc.fini();
		}
	}
}