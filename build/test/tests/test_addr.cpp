#include "stdafx.h"

#include <lax/net/detail/addr.h>
#include <catch.hpp>

using namespace lax::net;

TEST_CASE("ip address")
{
	SECTION("usage")
	{
		addr a1("127.0.0.1:7000");

		REQUIRE(a1.is_valid());
		REQUIRE(a1.port() == 7000);
		REQUIRE(a1.ip() == "127.0.0.1");
	}

	SECTION("valid addresses")
	{
		SECTION("any")
		{
			addr a1("0.0.0.0:7000");

			REQUIRE(a1.is_valid());
			REQUIRE(a1.port() == 7000);
			REQUIRE(a1.ip() == "0.0.0.0");
		}

		SECTION("small port")
		{
			addr a1("0.0.0.0:7");

			REQUIRE(a1.is_valid());
			REQUIRE(a1.port() == 7);
		}
	}

	SECTION("invalid addresses")
	{
		SECTION("missing ip part")
		{
			addr a1(".0.0.1:7000");

			REQUIRE(!a1.is_valid());
		}

		SECTION("missing port")
		{
			addr a1("0.0.0.1:");

			REQUIRE(!a1.is_valid());
		}

		SECTION("broken 1")
		{
			addr a1(".0.1:");

			REQUIRE(!a1.is_valid());
		}

		SECTION("space")
		{
			addr a1(" 127.0.0.1:8000 ");

			// 요청 문자열을 컨테이너 키로 사용할 수 있어 trim 지원 안 함

			REQUIRE(!a1.is_valid());
		}

		SECTION("space within")
		{
			addr a1(" 127.0.0 .1:8 000 ");

			// 요청 문자열을 컨테이너 키로 사용할 수 있어 trim 지원 안 함

			REQUIRE(!a1.is_valid());
		}
	}
}
