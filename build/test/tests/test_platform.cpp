#include "stdafx.h"

#include <lax/util/platform.h>
#include <catch.hpp>
#include <vector>

using namespace lax::util;

TEST_CASE("test platform")
{
	SECTION("get current dir")
	{
		auto dir = platform::get_current_dir();
	}

}