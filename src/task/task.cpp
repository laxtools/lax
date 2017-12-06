#include "stdafx.h"
#include <lax/task/task.hpp>
#include <lax/task/task_exceptions.hpp>

namespace lax {
namespace task {

task::task()
{
	check(state_ == state::constructed);
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

	return state_ == state::ready;
}

void task::execute(uint32_t runner_id)
{
	if (state_ != state::ready)
	{
		throw new exception_task_not_ready("task not ready in execute");
	}

	if (affinity_ && !has_right_affinity(runner_id))
	{
		throw new exception_task_invalid_affinity("invalid runner id");
	}

	last_runner_id_ = runner_id;

	execution_timer_.reset();

	on_execute();

	execution_count_++;
	last_execution_time_ = execution_timer_.elapsed();
	total_execution_time_ += last_execution_time_;
	average_execution_time_ = (average_execution_time_ + last_execution_time_) / 2;
}

void task::suspend()
{
	state_ = state::suspended;
}

void task::resume()
{
	state_ = state::ready;
}

void task::finish()
{
	state_ = state::finished;
}

bool task::on_start()
{
	return true;
}

void task::on_execute()
{
}

bool task::has_right_affinity(uint32_t runner_id) const 
{
	return affinity_ && (last_runner_id_ == runner_id || last_runner_id_ == 0);
}

} // task 
} // lax