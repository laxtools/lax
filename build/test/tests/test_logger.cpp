#include "stdafx.h"

#include <lax/util/logger.h>
#include <catch.hpp>

using namespace lax::util;

TEST_CASE("spdlog example")
{
	SECTION("typical usage")
	{
		auto log = spdlog::stdout_color_mt("console"); 

		log->info("Hello {0}", 333);

		auto l2 = spdlog::get("console");

		l2->warn("good or bad?");

		spdlog::drop("console");
	}

	SECTION("multiple sinks")
	{
		auto s1 = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
		auto s2 = std::make_shared<spdlog::sinks::daily_file_sink_mt>("log_", 0, 0);

		auto log = spdlog::create("main", { s1, s2 });

		log->info("multiple sinks");

		spdlog::drop("main");
	}

	SECTION("helper")
	{
		log_helper::retry_on_overflow = false; 

		log_helper::init();

		log_helper::get()->info("this can help some");
		log_helper::get()->warn("도움이 될 듯");
	}
}