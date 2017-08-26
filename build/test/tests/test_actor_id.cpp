#include "stdafx.h"

#include <lax/actor/actor.h>
#include <catch.hpp>

using namespace lax::actor;

TEST_CASE("actor id")
{
	SECTION("�������� ������ ���ΰ�?")
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
		//   - ���� �˸� ���� ���� ������ �� �ִ�. 
		//   - �˷��� actor�� ���ؼ� ã�� 
		//   - �װɷ� �����ϴ� ����� msgpack / json ��ο� �����Ѵ�. 
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