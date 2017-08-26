#include "stdafx.h"

#include <lax/util/object_pool.h>
#include <lax/util/simple_timer.h>
#include <lax/util/logger.h>
#include <catch.hpp>

namespace
{
struct simple
{
	char c[128];
	int v = 3;

	simple(int k) : v(k) {}
	~simple() { v = 0; }
};

}

using namespace lax::util;

TEST_CASE("object pool design")
{
	SECTION("design")
	{
		// 목표 : 
		// - auto ptr = pool.construct<entity>(); --> 풀에서 할당
		// - ptr.reset() --> 풀로 해제
		// 
	}

	SECTION("try class new/delete")
	{
		// Fail 주석: 
		// - class의 operator new / delete를 사용하는 방법은 이제 고전. 
	}

	SECTION("simple one")
	{
		object_pool<simple> pool;

		{
			auto s1 = pool.construct(9);
			REQUIRE(s1->v == 9);
		}

		REQUIRE(pool.unsafe_size() == 1);

		// ref를 통해 자동 해제. 
		// ref를 여러 곳에서 사용할 수 없음. 
	}

	SECTION("ref with shared_ptr")
	{
		object_pool<simple> pool;

		object_pool<simple>::shared_ref r1; 

		{
			auto s1 = pool.construct_shared(9);
			REQUIRE((*s1)->v == 9);
			REQUIRE(s1->get()->v == 9);
			REQUIRE(pool.get(s1)->v == 9);
			r1 = s1;
		}

		REQUIRE(pool.unsafe_size() == 0);

		r1.reset();

		REQUIRE(pool.unsafe_size() == 1);
	}
}

TEST_CASE("object pool perf with ref")
{
	const auto test_count = 10'000;

	object_pool<simple> pool;

	simple_timer timer;

	for (int i = 0; i<test_count; ++i)
	{
		auto s1 = pool.construct(9);

		auto v = s1->v;
	}

	log_helper::get()->info(
		"object pool. perf with ref: {0}",
		timer.elapsed()
	);

	log_helper::get()->flush();

	// release 9ms. on i7-2600K
}