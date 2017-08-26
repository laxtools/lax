#include "stdafx.h"
#include "runner.h"

namespace lax
{
namespace actor
{

runner::runner(ptr parent, id aid, ref::role role)
	: cluster_actor(parent, aid, role)
{
	thread_.swap(std::thread([this]()
	{
		this->run();
	})
	);
}

runner::~runner()
{
	stop_ = true;

	thread_.join();
}

void runner::run()
{
	while (!stop_)
	{
		auto current = timer_.elapsed();

		cluster_actor::process(current);

		if (frame_sleep_time_ > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(frame_sleep_time_));
		}

		++frame_;

		// TODO: process time check µî
	}
}

void runner::process(float tick)
{
	tick;

	// empty to hide
}

} // actor 
} // lax

