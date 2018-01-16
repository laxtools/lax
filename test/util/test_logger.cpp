#include "stdafx.h"
#include <catch.hpp>
#include <lax/util/logger.hpp>
#include <iostream>

using namespace lax::util;

TEST_CASE("logger")
{
	SECTION("basics")
	{
		log::get()->info("hello {0}", "abc");
	}

	SECTION("performance")
	{
	}
}