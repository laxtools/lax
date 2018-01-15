#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/detail/send_buffer.hpp>

using namespace lax::net;

TEST_CASE("send buffer")
{
	SECTION("basic case")
	{
		send_buffer<7> sb; 

		REQUIRE(sb.append("abc", 3) == 3);
		REQUIRE(sb.append("cde", 3) == 3);
		REQUIRE(sb.append("fgh", 3) == 3);

		REQUIRE(sb.size() == 9);
		REQUIRE(sb.get_write_seg() == 1);
		REQUIRE(sb.get_data_seg() == 1);

		for (int i = 0; i < 128; ++i)
		{
			REQUIRE(sb.append("aaa", 3) == 3);
			REQUIRE(sb.size() == 9 + (i + 1) * 3);
		}
	}

	SECTION("edge cases")
	{
		send_buffer<7> sb; 

		REQUIRE(sb.append("abcdefg", 7) == 7);
		REQUIRE(sb.get_write_seg() == 1);
		REQUIRE(sb.get_data_seg() == 0);

		auto lst = sb.transfer();

		REQUIRE(lst.size() == 1);
		REQUIRE(sb.size() == 0);
		REQUIRE(sb.get_write_seg() == 0);
		REQUIRE(sb.get_data_seg() == 0);

		sb.release(lst);
	}
}

