#include "stdafx.h"

#include <lax/actor/actor.h>
#include <lax/util/simple_timer.h>
#include <catch.hpp>

using namespace lax::actor;
#include "stdafx.h"

#include <lax/actor/actor.h>
#include <catch.hpp>

using namespace lax::actor;
using namespace lax::net;
using lax::util::simple_timer;

TEST_CASE("actor ref")
{
	SECTION("simple usage")
	{
		ref r1(id(3, 33));

		// start
		{
			json::Json data;
			r1.update(3, session::id(4, 33), 3, ref::state::unknown);

			REQUIRE(r1.get_last_update_tick() == 3);
			REQUIRE(r1.is_alive(10));
			REQUIRE(r1.get_state() == ref::state::unknown);
			REQUIRE(r1.get_up_time() == 0);
		}

		// up
		{
			json::Json data;
			r1.update(5, session::id(4, 33), 3, ref::state::up);

			REQUIRE(r1.get_last_update_tick() == 5);
			REQUIRE(!r1.is_alive(40));		// ttl Áö³ª¼­ alive ¾Æ´Ô
			REQUIRE(r1.get_state() == ref::state::up);
			REQUIRE(r1.get_up_time() == 2);
		}
		
		// shutdown
		{
			json::Json data;
			r1.update(8, session::id(4, 33), 3, ref::state::down);

			REQUIRE(r1.get_last_update_tick() == 8);
			REQUIRE(!r1.is_alive_state());
			REQUIRE(r1.get_state() == ref::state::down);
			REQUIRE(r1.get_up_time() == 5);
		}
	}
}