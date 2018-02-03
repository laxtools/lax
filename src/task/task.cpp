#include "stdafx.h"
#include <lax/task/task.hpp>
#include <lax/util/exception.hpp>
#include <lax/util/logger.hpp>

namespace lax {
namespace task {

task::task()
{
	VERIFY(state_ == state::constructed);
}

task::~task()
{
}

bool task::start()
{
	if (on_start())
	{
		state_ = state::ready;
	}

	util::log()->debug("task {} started", get_desc());

	return state_ == state::ready;
}

void task::execute(uint32_t runner_id)
{
	if (state_ != state::ready)
	{
		THROW("task not ready in execute");
	}

	if (affinity_ && !has_right_affinity(runner_id))
	{
		THROW("invalid runner affinity");
	}

	last_runner_id_ = runner_id;

	execution_timer_.reset();

	util::log()->trace("task {} executing...", get_desc());

	on_execute();

	++execution_count_;
	last_execution_time_ = execution_timer_.elapsed();
	total_execution_time_ += last_execution_time_;
	average_execution_time_ = (average_execution_time_ + last_execution_time_) / 2;

	util::log()->trace(
		"task {} executed in {} sec", get_desc(), last_execution_time_
	);
}

void task::finish()
{
	util::log()->debug("task {} finishing...", get_desc());

	on_finish();

	util::log()->debug("task {} finished", get_desc());

	state_ = state::finished;
}

bool task::on_start()
{
	return true;
}

void task::on_execute()
{
}

void task::on_finish()
{
}

bool task::has_right_affinity(uint32_t runner_id) const 
{
	return affinity_ && (last_runner_id_ == runner_id || last_runner_id_ == 0);
}

} // task 
} // lax