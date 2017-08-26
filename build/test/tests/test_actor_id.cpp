#include "stdafx.h"

#include <lax/actor/actor.h>
#include <catch.hpp>

using namespace lax::actor;

TEST_CASE("actor id")
{
	SECTION("무엇으로 구분할 것인가?")
	{
		// protocol 
		//   - msgpack 
		//   - json 
		//   - ...
		// 
		// msgpack 
		//   - key 
		// 
		// json 
		//   - key string

		//
		// id 
		//   - 서로 알면 보다 쉽게 전달할 수 있다. 
		//   - 알려진 actor들 통해서 찾고 
		//   - 그걸로 전달하는 방식을 msgpack / json 모두에 구현한다. 
		// 
	}

	SECTION("id design")
	{
		SECTION("simple usage")
		{
			actor::id id(3, 33);

			REQUIRE(id.is_valid());
			REQUIRE(id.local == 33);
			REQUIRE(id.node == 3);

			actor::id id2(id);

			REQUIRE(id2.local == 33);
		}

		SECTION("map key")
		{
			std::map<actor::id, int> tm;

			tm[actor::id(3, 33)] = 3;
			tm[actor::id(7, 77)] = 7;

			REQUIRE(tm[actor::id(3, 33)] == 3);
			REQUIRE(tm[actor::id(7, 77)] == 7);
		}

		SECTION("hashmap key")
		{
			std::unordered_map<actor::id, int> tm;

			tm[actor::id(3, 33)] = 3;
			tm[actor::id(7, 77)] = 7;

			REQUIRE(tm[actor::id(3, 33)] == 3);
			REQUIRE(tm[actor::id(7, 77)] == 7);
		}
	}	
		
}