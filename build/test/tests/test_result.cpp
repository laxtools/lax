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
			// rvalue�� ����ϱ� ���� �� ���� ������ �Լ��� �������� �ʴ´�. 
			// lvalue ������ rvalue ������ ����Ѵ�. 

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

			// result���� std::vector�� rvalue �����ڸ� ���� �̵���

			REQUIRE(rc);
			REQUIRE(rc.value[0] == 3);


			// �����丮: 
			// - ���ÿ� �Ʒ� ������ ���ǵǸ� ��ȣ�ϴٰ� ����
			//    result(Code acode, Value&& avalue)
			//   �̴� �� ���� ������ ���� ��쿡 �ش��� 
			//   ������ lvalue, rvalue ������ ������ ����ϸ� �ȴ�.
		}
	}
} 
