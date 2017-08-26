#include "stdafx.h"

#include <catch.hpp>
#include <lax/util/sequence.h>
#include <lax/util/simple_timer.h>

#include <iostream>
#include <mutex>

using namespace lax::util;

TEST_CASE("id pool")
{
	SECTION("usage")
	{
		sequence<uint32_t, std::recursive_mutex>  pool(1, 1024, 100);

		REQUIRE(pool.queue_size() == 100);
		REQUIRE(pool.current() == 101);

		REQUIRE(pool.next() == 1);
		REQUIRE(pool.queue_size() == 99);

		REQUIRE(pool.next() == 2);
		REQUIRE(pool.queue_size() == 98);
	}

	SECTION("performance")
	{
		const auto test_count = 100'000; 

		sequence<uint32_t>  pool(1, 100001); // begin부터 end-1까지만 유효

		simple_timer timer;

		{
			for (int i = 0; i < test_count; ++i)
			{
				pool.next();
			}
		}

		std::cout << "elapsed: " << timer.elapsed() << std::endl;
	}
}