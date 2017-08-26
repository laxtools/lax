#pragma once

#include <chrono>

namespace lax
{
namespace util
{

class simple_timer
{
	using clock = std::chrono::steady_clock;

public: 
	simple_timer()
	{
		start_ = clock::now();
	}

	float elapsed() const
	{
		auto lap = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_);

		return lap.count() * 0.001f;
	}

private: 
	clock::time_point start_;
};

} // util 
} // lax
