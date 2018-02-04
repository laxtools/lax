#include "stdafx.h"
#include <lax/task/task_scheduler.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace task
{

task_scheduler::task_scheduler()
{

}

task_scheduler::~task_scheduler()
{

}

bool task_scheduler::start(const config& cfg)
{
	VERIFY(cfg.runner_count > 0);

	config_ = cfg; 

	for (auto i = 0; i < cfg.runner_count; ++i)
	{
		task_runner::config config; 

		config.idle_check_threshold_time = config_.idle_check_threshold_time;
		config.single_loop_run_limit = config_.single_loop_run_limit;

		auto runner = std::make_shared<task_runner>(*this, i+1);
		auto rc = runner->start(config);

		if (rc)
		{
			runners_.push_back(runner);
		}
	}

	util::log()->info(
		"task_scheduler started. {} runners", get_runner_count()
	);

	return get_runner_count() > 0;
}

void task_scheduler::schedule(task::ptr task)
{
	if (config_.start_task_when_schedule_called)
	{
		if (task->get_state() == task::state::constructed)
		{
			(void)task->start();
		}
	}

	if (task->get_state() == task::state::ready)
	{
		queue_.push(task);
	}
	else
	{
		// task finished. what to do?
	}

	if (config_.run_schedule_when_schedule_called)
	{
		run_schedule();
	}
}

void task_scheduler::execute()
{
	run_schedule();
}

void task_scheduler::finish()
{
	util::log()->info( "task_scheduler finishing..." );

	for (auto& r : runners_)
	{
		r->finish();
	}

	task::ptr task;

	while (queue_.pop(task))
	{
		if (task->get_state() == task::state::ready )
		{
			task->finish();
		}
	}

	util::log()->info( "task_scheduler finished");
}

void task_scheduler::run_schedule()
{
	task::ptr task;

	std::size_t schedule_count = 0;

	while (queue_.pop(task))
	{
		VERIFY(task->get_state() == task::state::ready);

		if (task->is_runner_affinity_set())
		{
			auto id = task->get_last_runner_id();

			if (id == 0)
			{
				id = get_target_runner_id();
			}

			VERIFY(id > 0);
			runners_[id-1]->push(task);
		}
		else
		{
			auto id = get_target_runner_id();
			VERIFY(id > 0);
			runners_[id]->push(task);
		}

		++schedule_count;
	}

	util::log()->debug("{} scheduled", schedule_count);
}

std::size_t task_scheduler::get_task_count() const
{
	auto s1 = queue_.unsafe_size();

	for (auto& r : runners_)
	{
		s1 += r->get_task_count();
	}

	return s1;
}

uint32_t task_scheduler::get_target_runner_id() const
{
	bool schedule_policy_min_task_count = true;

	uint32_t min_task_count = UINT32_MAX;
	unsigned int cand_id = rand() % runners_.size();

	for (int i = 0; i < runners_.size(); ++i)
	{
		auto task_count = runners_[i]->get_task_count();

		if (task_count < min_task_count)
		{
			cand_id = i + 1;
			min_task_count = static_cast<uint32_t>(task_count);
		}
	}

	return cand_id;
}

void task_scheduler::pass(task::ptr task)
{
	queue_.push(task);
}

} // task 
} // lax
