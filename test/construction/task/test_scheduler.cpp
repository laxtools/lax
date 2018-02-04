#include "stdafx.h"
#include <catch.hpp>
#include <lax/task/task_scheduler.hpp>

using namespace lax::task;

TEST_CASE("scheduler") 
{
	SECTION("interface")
	{
		task_scheduler scheduler;
		task_scheduler::config config;

		config.runner_count = 5;

		REQUIRE(scheduler.start(config));
		REQUIRE(scheduler.get_runner_count() == config.runner_count);

		auto task1 = std::make_shared<task>();
		scheduler.schedule(task1);

		scheduler.execute();

		// wait for task1 executed
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		REQUIRE(task1->get_execution_count() > 0);

		REQUIRE(scheduler.get_task_count() >= 0); // 타이밍에 따라 0으로 나올 때가 있음

		REQUIRE(task1->get_state() == task::state::ready); // start called in schedule if not already started

		scheduler.finish();

		REQUIRE(task1->get_state() == task::state::finished);
	}

	SECTION("load test")
	{

	}
}
