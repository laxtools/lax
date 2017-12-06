#include "stdafx.h"
#include <lax/task/task_scheduler.hpp>

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
	check(cfg.runner_count > 0);

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

	queue_.push(task);

	if (config_.run_schedule_when_schedule_called)
	{
		run_schedule();
	}
}

void task_scheduler::run()
{
	run_schedule();
}

void task_scheduler::finish()
{
	for (auto& r : runners_)
	{
		r->finish();
	}

	task::ptr task;

	while (queue_.pop(task))
	{
		if (task->get_state() == task::state::ready )
		{
			if (config_.finish_ready_task_during_finish)
			{
				task->finish();
			}
		}
	}
}

void task_scheduler::run_schedule()
{
	task::ptr task;

	while (queue_.pop(task))
	{
		if (task->is_affinity_set())
		{
			auto id = task->get_last_runner_id();

			if (id == 0)
			{
				id = get_target_runner_id();
			}

			check(id > 0);
			runners_[id-1]->push(task);
		}
		else
		{
			auto id = get_target_runner_id();
			check(id > 0);
			runners_[id]->push(task);
		}
	}
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
	// 가장 태스크 개수가 작은 runner에 다음 태스크 할당 
	// TODO: 예상 실행 시간으로 할당하는 알고리즘 고려

	bool schedule_policy_min_task_count = true;
	// schedule_policy_min_task_run_time

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

} // task 
} // lax
