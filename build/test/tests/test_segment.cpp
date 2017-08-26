#include "stdafx.h"

#include <lax/net/detail/segment.h>
#include <lax/net/detail/segment_buffer.h>
#include <catch.hpp>

using namespace lax::net;

TEST_CASE("segment")
{
	SECTION("usage")
	{
		segment<32> s1;

		// init
		{
			auto p = s1.get();

			REQUIRE(p.data());
			REQUIRE(p.size() == 0);
		}

		// append
		{
			REQUIRE(s1.append("hello", 5) == 5);

			auto p = s1.get();
			REQUIRE(p.size() == 5);
		}

		// rewind
		{
			s1.rewind();

			REQUIRE(s1.get().size() == 0);
		}

		// overflow
		{
			char v[32];

			::memset(v, 1, 32);

			REQUIRE(s1.append(v, 32) == 32);
			REQUIRE(s1.append(v, 32) == 0);
			REQUIRE(s1.get().size() == 32);
		}
	}

	SECTION("pop front")
	{
		// normal
		{
			segment<32> s1;

			s1.append("012345678", 9);
			s1.pop_front(2);

			REQUIRE(s1.get().data()[0] == '2');
		}

		// pop more 
		{
			segment<32> s1;

			s1.append("012345678", 9);
			s1.pop_front(100);

			s1.append("9", 1);

			REQUIRE(s1.get().data()[0] == '9');
		}
	}
}

