#include "stdafx.h"
#include <catch.hpp>

#define return_do(c, v) if ((c)) v

TEST_CASE("c++ macros")
{

	SECTION("return_do")
	{
		return_do(
			3 == (5 + 3),
			auto p = [](int a, int v) {}; 
			return;
		);
	}
}