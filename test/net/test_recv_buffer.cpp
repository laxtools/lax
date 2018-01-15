#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/detail/recv_buffer.hpp>

using namespace lax::net;

TEST_CASE("recv buffer")
{
	SECTION("basic usage")
	{
		recv_buffer rb; 

		REQUIRE(rb.append("abc", 3) == 3);
		REQUIRE(rb.data()[0] == 'a');
	}

	SECTION("resizing")
	{
		recv_buffer rb; 

		for (int i = 0; i < 1024; ++i)
		{
			rb.append("a", 1);
		}

		REQUIRE(rb.capacity() == 1024);
		REQUIRE(rb.get_pool(1024)->get_stat().alloc_count == 1);
	}

	SECTION("pop front")
	{
		recv_buffer rb; 

		REQUIRE(rb.append("abc", 3) == 3);
		REQUIRE(rb.data()[0] == 'a');

		rb.pop_front(2); 

		REQUIRE(rb.data()[0] == 'c');
	}
}
