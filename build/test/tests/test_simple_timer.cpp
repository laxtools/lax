#include "stdafx.h"

#include <lax/util/simple_timer.h>

#include <catch.hpp>
#include <thread>

TEST_CASE("timer correctness", "[simple timer]")
{
	SECTION("basic")
	{
		lax::util::simple_timer timer;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		auto elapsed = timer.elapsed();
	
		REQUIRE(elapsed >= 0.045);
		REQUIRE(elapsed <= 0.075); // debug 모드에서 꽤 차이가 난다. 왜?
	}

}