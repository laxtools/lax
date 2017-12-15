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

		task1.set_runner_affinity();

		CHECK_THROWS(task1.execute(2)); // exception_task_invalid_affinity

		task1.execute(1);
	}

}
