#include "stdafx.h"
#include <catch.hpp>
#include <lax/channel/channel.hpp>

using namespace lax::channel;

TEST_CASE("test channel")
{
	// 


	SECTION("details")
	{
		SECTION("sub")
		{
			int value = 0;
			auto cb = [&value](message::ptr m) { value++; };
			auto true_ = [](message::ptr m) { return true; };

			sub s1( 1, topic(1), true_, cb, sub::mode::delayed);

			auto mp = std::make_shared<message>();
			mp->set_topic(topic(1));

			REQUIRE(s1.post(mp));
			REQUIRE(value == 1);

			REQUIRE(s1.post(mp));
			REQUIRE(value == 2);
		}

		SECTION("sub_map")
		{
			SECTION("delayed")
			{
				sub_map sm(false);

				int value = 0;
				auto cb = [&value](message::ptr m) { value++; };

				auto sk = sm.subscribe(topic(1), cb, sub::mode::delayed);
				REQUIRE(sk > 0);

				auto mp = std::make_shared<message>();
				mp->set_topic(topic(1));

				auto cnt = sm.post(mp, sub::mode::delayed);
				REQUIRE(cnt == 1);
				REQUIRE(value == 1);
			}

			SECTION("immediate")
			{
				sub_map sm(false);

				int value = 0;
				auto cb = [&value](message::ptr m) { value++; };

				auto sk = sm.subscribe(topic(1), cb, sub::mode::immediate);
				REQUIRE(sk > 0);

				auto mp = std::make_shared<message>();
				mp->set_topic(topic(1));

				auto cnt1 = sm.post(mp, sub::mode::delayed);
				REQUIRE(cnt1 == 0);
				REQUIRE(value == 0);

				auto cnt2 = sm.post(mp, sub::mode::immediate);
				REQUIRE(cnt2 == 1);
				REQUIRE(value == 1);
			}

			SECTION("multiple subs")
			{
				sub_map sm(false);

				int value = 0;
				auto cb = [&value](message::ptr m) { value++; };

				auto sk1 = sm.subscribe(topic(1), cb, sub::mode::immediate);
				REQUIRE(sk1 > 0);

				auto sk2 = sm.subscribe(topic(1), cb, sub::mode::immediate);
				REQUIRE(sk2 > 0);

				auto mp = std::make_shared<message>();
				mp->set_topic(topic(1));

				auto cnt1 = sm.post(mp, sub::mode::delayed);
				REQUIRE(cnt1 == 0);
				REQUIRE(value == 0);

				auto cnt2 = sm.post(mp, sub::mode::immediate);
				REQUIRE(cnt2 == 2);
				REQUIRE(value == 2);
			}

			SECTION("unsub")
			{
				sub_map sm(false);

				int value = 0;
				auto cb = [&value](message::ptr m) { value++; };

				auto sk1 = sm.subscribe(topic(1), cb, sub::mode::immediate);
				REQUIRE(sk1 > 0);

				auto sk2 = sm.subscribe(topic(1), cb, sub::mode::immediate);
				REQUIRE(sk2 > 0);

				auto mp = std::make_shared<message>();
				mp->set_topic(topic(1));

				auto cnt1 = sm.post(mp, sub::mode::delayed);
				REQUIRE(cnt1 == 0);
				REQUIRE(value == 0);

				REQUIRE(sm.unsubscribe(sk1));

				auto cnt2 = sm.post(mp, sub::mode::immediate);
				REQUIRE(cnt2 == 1);
				REQUIRE(value == 1);
			}
		}

		SECTION("channel_map")
		{
			auto cp = channel::create("test1", channel::config());
			REQUIRE(cp);

			auto cp2 = channel::find("test1");
			REQUIRE(cp2);
			REQUIRE(cp2->get_key() == cp->get_key());
		}
	}

	SECTION("usages")
	{
		SECTION("basic")
		{
			auto cp = channel::create("test2", channel::config());
			REQUIRE(!!cp); 

			// sub 
			int value = 0;
			auto cb = [&value](message::ptr m) { value++; };

			auto sk1 = cp->subscribe(topic(1), cb, sub::mode::delayed);
			REQUIRE(sk1 > 0);

			// push 
			auto mp = std::make_shared<message>();
			mp->set_topic(topic(1));

			REQUIRE(cp->push(mp) == 0);

			// post
			REQUIRE(cp->execute() == 1);

			REQUIRE(value == 1);

			REQUIRE(channel::destroy("test2"));

			REQUIRE(!channel::find("test2"));

			// cp는 이 블록 안에서 유효하다.
		}
	}

	SECTION("group posting")
	{
		auto cp = channel::create("test2", channel::config());
		REQUIRE(!!cp);

		// sub 
		int value = 0;
		auto cb = [&value](message::ptr m) { value++; };

		auto sk1 = cp->subscribe(topic(1, 0), cb, sub::mode::delayed);
		REQUIRE(sk1 > 0);

		// push 
		auto mp = std::make_shared<message>();
		mp->set_topic(topic(1, 1));

		REQUIRE(cp->push(mp) == 0);

		// post
		REQUIRE(cp->execute() == 1);

		REQUIRE(value == 1);

		REQUIRE(channel::destroy("test2"));

		REQUIRE(!channel::find("test2"));
	}

	SECTION("basic performance")
	{
		const int TEST_COUNT = 1000;

		auto cfg = channel::config();

		// cfg.use_lock = false;

		auto cp = channel::create("test2", cfg);
		REQUIRE(!!cp);

		// sub 
		int value = 0;
		auto cb = [&value](message::ptr m) { value++; };

		for (int i = 0; i < 1000; ++i)
		{
			auto sk1 = cp->subscribe(topic(i), cb, sub::mode::delayed);
			REQUIRE(sk1 > 0);
		}

		auto mp = std::make_shared<message>();
		mp->set_topic(topic(1));

		for (int i = 0; i < TEST_COUNT; ++i)
		{
			// push 
			REQUIRE(cp->push(mp) == 0);

			// post
			REQUIRE(cp->execute() == 1);

			REQUIRE(value == i+1);
		}

		REQUIRE(channel::destroy("test2"));

		// 1백만번 push/post에 4초 정도. 
		// subscribe는 1천개 정도. 
		// lock 사용 여부는 단일 쓰레드에서는 크게 영향 없다.

		// 프로파일러로 보면 catch의 오버헤드가 크다. 
		// 따라서, 별도 프로그램으로 최적화를 진행한다.
	}

}