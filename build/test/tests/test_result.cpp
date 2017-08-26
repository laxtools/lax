#include "stdafx.h"

#include <lax/util/result.h>
#include <catch.hpp>
#include <vector>

using namespace lax::util;

TEST_CASE("result")
{
	SECTION("usage")
	{
		using result = result<int, int>;

		auto func = []() { return result(0, 3); };

		REQUIRE(func());
	}

	SECTION("pointer")
	{
		using result = result<int, int*>;

		int v = 3;

		auto func = [&v]() { return result(0, &v); };

		auto rc = func();

		REQUIRE(rc);
		REQUIRE(*rc.value == 3);
	}

	SECTION("bool")
	{
		using result = result<bool, int>;

		auto func = []() { return result(true, 3); };

		auto rc = func();

		REQUIRE(rc);
		REQUIRE(rc.value == 3);
	}

	SECTION("container")
	{
		SECTION("value copy")
		{
			using result = result<bool, std::vector<int>>;

			auto func = []() {
				std::vector<int> k;
				k.push_back(3);
				return result(true, k);
			};

			auto rc = func();

			REQUIRE(rc);
			REQUIRE(rc.value[0] == 3);
		}

		SECTION("reference")
		{
			// rvalue를 사용하기 위해 값 복사 버전의 함수를 제공하지 않는다. 
			// lvalue 버전과 rvalue 버전을 사용한다. 

			/*
			using result = result<bool, std::vector<int>&>;

			std::vector<int> k;

			auto func = [&k]() {
				k.push_back(3);
				return result(0, k);
			};

			auto rc = func();

			REQUIRE(rc);
			REQUIRE(rc.value[0] == 3);
			*/
		}

		SECTION("rvalue")
		{
			using result = result<bool, std::vector<int>>;

			auto func = []() {
				std::vector<int> k;
				k.push_back(3);
				return result(true, std::move(k));
			};

			auto rc = func();

			// result에서 std::vector의 rvalue 생성자를 통해 이동됨

			REQUIRE(rc);
			REQUIRE(rc.value[0] == 3);


			// 히스토리: 
			// - 동시에 아래 생성자 정의되면 모호하다고 나옴
			//    result(Code acode, Value&& avalue)
			//   이는 값 복사 버전이 있을 경우에 해당함 
			//   이제는 lvalue, rvalue 버전을 나눠서 사용하면 된다.
		}
	}
} 
