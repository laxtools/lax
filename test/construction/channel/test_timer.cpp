#include "stdafx.h"
#include <catch.hpp>
#include <lax/channel/timer.hpp>
#include <chrono>
#include <thread>

using namespace lax::channel;

TEST_CASE("timer test")
{
	SECTION("basic usage")
	{
		timer tm;

		auto id = tm.set(0.3f);

		std::size_t count = 0;
 
		tm.add(id, [&count](timer::id_t id) { ++count; });

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		tm.update();

		CHECK(count == 1);

		tm.update();

		CHECK(count == 1);

		tm.cancel(id);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		tm.update();

		CHECK(tm.get_req_count() == 0);
	}
}