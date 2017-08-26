#include "stdafx.h"

#include <catch.hpp>
#include <spdlog/spdlog.h>

namespace spd = spdlog;

TEST_CASE("design log")
{

	SECTION("learning spdlog")
	{
		SECTION("console log")
		{
			std::size_t q_size = 4096; //queue size must be power of 2
			spd::set_async_mode(q_size);

			// simple cases
			auto console = spd::stdout_color_mt("console");
			console->info("Welcome to spdlog!");
			console->error("Some error message with arg{}..", 1);

			// Formatting examples
			console->warn("Easy padding in numbers like {:08d}", 12);
			console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
			console->info("Support for floats {:03.2f}", 1.23456);
			console->info("Positional args are {1} {0}..", "too", "supported");
			console->info("{:<30}", "left aligned");
		}

		SECTION("thinking on exception")
		{
			// can be viewed as runtime bomb vs. as an easy way to handle odd cases. 
		}

		SECTION("more features")
		{

		}
	}

}

