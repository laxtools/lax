#include "stdafx.h"

#include <catch.hpp>

TEST_CASE("design convention")
{
	SECTION("naming")
	{
		// asio convention

		// spdlog, caf has similar one

		// trailing underscore for member variables
	}

	SECTION("use const when possible")
	{
		// use mutable when required 
	}

	SECTION("use reference when possible")
	{
	}

	SECTION("use smart pointers except extremely unproper cases")
	{
	}

	SECTION("use error class (type) to report errors")
	{

	}

	SECTION("use exception only when essential")
	{
		// 필수적인 경우는 : 
		//   - 없을 듯...
	}

	SECTION("use ppltasks.h writing flow")
	{
		// 주석. 타잎 기반 (type safety) 
		// naming. 
		// 
	}

}

