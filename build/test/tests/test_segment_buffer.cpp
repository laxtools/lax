#include "stdafx.h"

#include <lax/net/detail/segment_buffer.h>
#include <catch.hpp>

using namespace lax::net;

TEST_CASE("segment buffer write")
{
	SECTION("aligned to length")
	{
		segment_buffer<32> sb;

		for (int i = 0; i < 128; ++i)
		{
			REQUIRE(sb.append("12345678", 8) == 8);
		}

		REQUIRE(sb.size() == 8 * 128);
		REQUIRE(sb.get_write_seg() == (8 * 128) / 32);
	}

	SECTION("unaligned to length")
	{
		segment_buffer<32> sb;

		for (int i = 0; i < 128; ++i)
		{
			REQUIRE(sb.append("hello", 5) == 5);
		}

		REQUIRE(sb.size() == 5 * 128);
		REQUIRE(sb.get_write_seg() == (5 * 128) / 32);
	}

	SECTION("large increase")
	{
		segment_buffer<32> sb;

		char c[128];
		::memset(c, 1, 128);

		for (int i = 0; i < 128; ++i)
		{
			REQUIRE(sb.append(c, 128) == 128);
		}

		REQUIRE(sb.size() == 128 * 128);
		REQUIRE(sb.get_write_seg() == (128 * 128) / 32);
	}

	SECTION("one byte increase")
	{
		segment_buffer<32> sb;

		for (int i = 0; i < 2048; ++i)
		{
			REQUIRE(sb.append("a", 1) == 1);
		}

		REQUIRE(sb.size() == 1 * 2048);
		REQUIRE(sb.get_write_seg() == (1 * 2048) / 32);
	}

	SECTION("one size buffer")
	{
		segment_buffer<1> sb;

		for (int i = 0; i < 2048; ++i)
		{
			REQUIRE(sb.append("a", 1) == 1);
		}

		REQUIRE(sb.size() == 1 * 2048);
		REQUIRE(sb.get_write_seg() == (1 * 2048) / 1);
	}

	SECTION("non modular")
	{
		segment_buffer<17> sb;

		for (int i = 0; i < 1127; ++i)
		{
			REQUIRE(sb.append("abc", 3) == 3);
		}

		REQUIRE(sb.size() == 3 * 1127);
		REQUIRE(sb.get_write_seg() == (3 * 1127) / 17);
	}
}

TEST_CASE("segment_buffer read")
{
	SECTION("basic")
	{
		segment_buffer<32> sb;

		for (int i = 0; i < 2048; ++i)
		{
			REQUIRE(sb.append("abc", 3) == 3);
		}

		REQUIRE(sb.size() == 3 * 2048);
		REQUIRE(sb.get_write_seg() == (3 * 2048) / 32);

		auto lst = sb.transfer();

		REQUIRE(lst.size() == (3 * 2048) / 32);

		sb.release(lst);

		REQUIRE(sb.size() == 0);
	}
}