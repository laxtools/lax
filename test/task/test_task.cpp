#include "stdafx.h"
#include <catch.hpp>
#include <lax/task/task.hpp>

using namespace lax::task;

TEST_CASE("task") 
{
	SECTION("interface")
	{
		task task1; 

		REQUIRE(task1.start());
		REQUIRE(task1.get_state() == task::state::ready);

		task1.execute(1);

		REQUIRE(task1.get_last_runner_id() == 1);

		task1.suspend();
		REQUIRE(task1.get_state() == task::state::suspended);

		CHECK_THROWS(task1.execute(1)); // exception_task_not_ready

		task1.set_affinity();

		task1.resume();
		CHECK_THROWS(task1.execute(2)); // exception_task_invalid_affinity

		task1.execute(1);
	}

}
