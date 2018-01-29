#include "stdafx.h"
#include <catch.hpp>
#include <lax/channel/dispatcher.hpp>

using namespace lax::channel;

TEST_CASE("dispatcher")
{

	SECTION("basics")
	{
		dispatcher d; 

		int value = 0;
		auto cb = [&value](message::ptr m) { value++; };

		auto rc = d.subscribe(topic(1), cb);
		REQUIRE(rc);

		auto mp = std::make_shared<message>();
		mp->set_topic(topic(1));

		auto cnt1 = d.post(mp);
		REQUIRE(cnt1 == 1);
		REQUIRE(value == 1);

		auto cnt2 = d.post(mp);
		REQUIRE(cnt2 == 1);
		REQUIRE(value == 2);
	}

	SECTION("performance")
	{
		const int TEST_COUNT = 1000;

		dispatcher d; 

		int value = 0;
		auto cb = [&value](message::ptr m) { value++; };

		for (int i = 0; i < 1000; ++i)
		{
			auto rc = d.subscribe(topic(i+1), cb);
			REQUIRE(rc);
		}

		auto mp = std::make_shared<message>();
		mp->set_topic(topic(1));

		for (int i = 0; i < TEST_COUNT; ++i)
		{
			auto cnt1 = d.post(mp);
			REQUIRE(cnt1 == 1);
		}
	}
}