#include "stdafx.h"

#include <lax/actor/id.h>
#include <lax/actor/detail/index.h>
#include <lax/actor/detail/multi_index.h>
#include <lax/util/simple_timer.h>
#include <lax/util/logger.h>

#include <catch.hpp>
#include <map>

using namespace lax::actor;
using namespace lax::util;

TEST_CASE("test directory index")
{
	SECTION("index")
	{
		index<int, int> index;

		REQUIRE(index.link(3, 3));
		REQUIRE(index.link(3, 3) == false);

		REQUIRE(index.link(7, 1));

		index.unlink(3);

		REQUIRE(index.find(3) == false);
		REQUIRE(index.find(7).value == 1);
	}

	SECTION("multi_index")
	{
		using idx = multi_index<int, int>; 
		
		idx index;

		REQUIRE(index.link(3, 3));
		REQUIRE(index.link(3, 5));

		REQUIRE(index.link(7, 1));
		REQUIRE(index.link(7, 2));

		idx::vec vec;

		REQUIRE(index.find(3, vec));
		REQUIRE(vec[0] == 3);
		REQUIRE(vec[1] == 5);

		REQUIRE(index.unlink(7, 1));

		vec.clear();
		REQUIRE(index.find(7, vec));
		REQUIRE(vec.size() == 1);
		REQUIRE(vec[0] == 2);
	}

	SECTION("multi_index simple performance")
	{
		using idx = multi_index<int, int>;

		idx index;

#ifdef _DEBUG
		const int test_count = 1000;
		const int entry_count = 600;
#else
		const int test_count = 100000;
		const int entry_count = 60000;
#endif

		simple_timer timer;

		/// setup link 
		for (int i = 0; i < entry_count; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				index.link(entry_count, j);
			}
		}

		idx::vec vec;

		for (int i = 0; i < test_count; ++i)
		{
			index.find(std::rand() % entry_count, vec);
		}

		log()->info("multi_index. elapsed: {0}", timer.elapsed());

		// 6만 엔트리 10만번 검색. Release:  0.098초. Debug: 12초 
	}
}


TEST_CASE("actor id in result")
{
	

	SECTION("result with actor id")
	{
		auto rc = result<bool, id>(false, id(3));
	}
}

TEST_CASE("understanding multimap")
{
	SECTION("insert / find / erase")
	{
		using map = std::multimap<int, int>;

		map ints; 

		ints.insert(map::value_type(1, 1));
		ints.insert(map::value_type(1, 2));
		ints.insert(map::value_type(1, 3));

		ints.insert(map::value_type(2, 1));
		ints.insert(map::value_type(2, 2));
		ints.insert(map::value_type(2, 3));
		
		auto lb = ints.lower_bound(1);
		auto ub = ints.upper_bound(1);

		int count = 0;
		for (auto iter = lb; iter != ub; ++iter)
		{
			count++;
		}

		REQUIRE(count == 3);

		for (auto iter = lb; iter != ub; ++iter)
		{
			if (iter->second == 2)
			{
				ints.erase(iter);
				break;
			}
		}

		REQUIRE(ints.size() == 5);
	}

}