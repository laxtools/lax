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
		// ��ǥ : 
		// - auto ptr = pool.construct<entity>(); --> Ǯ���� �Ҵ�
		// - ptr.reset() --> Ǯ�� ����
		// 
	}

	SECTION("try class new/delete")
	{
		// Fail �ּ�: 
		// - class�� operator new / delete�� ����ϴ� ����� ���� ����. 
	}

	SECTION("simple one")
	{
		object_pool<simple> pool;

		{
			auto s1 = pool.construct(9);
			REQUIRE(s1->v == 9);
		}

		REQUIRE(pool.unsafe_size() == 1);

		// ref�� ���� �ڵ� ����. 
		// ref�� ���� ������ ����� �� ����. 
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