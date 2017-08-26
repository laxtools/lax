#include "stdafx.h"

#include <lax/util/concurrent_queue.h>
#include <lax/util/simple_timer.h>
#include <lax/util/logger.h>
#include <catch.hpp>

using namespace lax::util;

namespace
{
struct simple
{
	simple(int k) : v(k) {}
	int v = 0;
};
}

TEST_CASE("concurernt queue")
{
	SECTION("usage")
	{
		SECTION("value")
		{
			concurrent_queue<int> q;

			q.push(3);

			int v = 0;

			REQUIRE(q.pop(v));
			REQUIRE(v == 3);
			REQUIRE(q.unsafe_size() == 0);
		}

		SECTION("pointer")
		{
			concurrent_queue<int*> q;

			q.push(new int(3));

			int* v = nullptr;

			REQUIRE(q.pop(v));
			REQUIRE(*v == 3);
			REQUIRE(q.unsafe_size() == 0);
		}

		SECTION("shared ptr")
		{
			concurrent_queue<std::shared_ptr<simple>> q;

			q.push(std::make_shared<simple>(3));

			std::shared_ptr<simple> ptr;

			REQUIRE(q.pop(ptr));
			REQUIRE(ptr->v == 3);
			REQUIRE(q.unsafe_size() == 0);
		}
	}

	SECTION("perf")
	{
		const auto test_count = 100'000;

		simple_timer timer;

		concurrent_queue<int> q;

		for (int i = 0; i < test_count; ++i)
		{
			q.push(3);
		}

		for (int i = 0; i < test_count; ++i)
		{
			int v = 0;
			q.pop(v);
		}

		log_helper::get()->info(
			"concurrent_queue. perf: {0}", 
			timer.elapsed()
		);

		// release: 5ms. on i7-2600K. 
	}
}