#include "stdafx.h"
#include "catch.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <map>

namespace ex 
{
struct perf_span
{
	perf_span()
		: begin_(std::chrono::high_resolution_clock::now())
	{
	}

	void reset()
	{
		begin_ = std::chrono::high_resolution_clock::now();
	}

	float elapsed() const
	{
		std::chrono::duration<float> span = std::chrono::high_resolution_clock::now() - begin_;

		return span.count();
	}

	std::chrono::high_resolution_clock::time_point begin_;
};

}

TEST_CASE("high resolution", "clock")
{
	SECTION("simple usage")
	{
		auto clk = std::chrono::high_resolution_clock::now();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		auto now = std::chrono::high_resolution_clock::now();
	
		std::chrono::duration<float> span = (now - clk);

		std::cout << "seconds: " << span.count() << std::endl;
	}

	SECTION("simple timer")
	{
		ex::perf_span pspan;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		std::cout << "seconds: " << pspan.elapsed() << std::endl;
	}

	SECTION("c++ test")
	{
		std::map<int, std::string> hello;

		auto iter = hello.find(3);

		REQUIRE(iter->second == "hello");

		int v = 600'000;
	}
}