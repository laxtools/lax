#include "stdafx.h"
#include <lax/task/detail/task_runner.hpp>
#include <lax/task/task_scheduler.hpp>
#include <lax/util/simple_timer.hpp>

namespace lax
{
namespace task
{

task_runner::task_runner(task_scheduler& scheduler, uint32_t id)
	: scheduler_(scheduler)
	, id_(id)
{
}

task_runner::~task_runner()
{
	if (!stop_)
	{
		finish();
	}
}

bool task_runner::start(const config& cfg)
{
	check(cfg.idle_check_threshold_time >= 0.f);
	check(cfg.single_loop_run_limit >= 0);
	check(stop_);

	config_ = cfg;
	stop_ = false;

	std::thread([this]() { run(); }).swap(thread_);

	return true;
}

void task_runner::push(task::ptr task)
{
	queue_.push(task);
}

void task_runner::finish()
{
	check(!stop_);

	stop_ = true;
	thread_.join();

	// 종료 후 남은 태스크들을 스케줄러에 넘김
	task::ptr task; 

	while (queue_.pop(task))
	{
		scheduler_.pass(task);
	}
}

void task_runner::run()
{
	util::simple_timer loop_timer_;

	while (!stop_)
	{
		unsigned int loop_count = 0;
		loop_timer_.reset();
		
		task::ptr task; 

		while (queue_.pop(task))
		{
			task->execute(id_);

			if (!stop_)
			{
				scheduler_.schedule(task);
			}
			else
			{
				scheduler_.pass(task);
			}

			++loop_count;
			++task_run_count_;

			if (config_.single_loop_run_limit > 0 && 
				loop_count >= config_.single_loop_run_limit)
			{
				break;
			}
		}
		
		last_loop_time_ = loop_timer_.elapsed();
		total_loop_time_ += last_loop_time_;
		average_loop_time_ = (last_loop_time_ + average_loop_time_) / 2;

		if (loop_count == 0 || last_loop_time_ < config_.idle_check_threshold_time)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

} // task 
} // lax

