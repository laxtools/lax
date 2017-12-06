#pragma once 

#include <lax/util/concurrent_queue.hpp>
#include <lax/task/task.hpp>
#include <atomic>
#include <thread>

namespace lax
{
namespace task
{

class task_scheduler;

class task_runner
{
public: 
	struct config
	{
		/// 기본 값 1ms. 이보다 짧게 걸리면 1ms 쉬기
		float idle_check_threshold_time = 0.001f;   

		/// 0 : 무한. 숫자 : 개수 제한
		unsigned int single_loop_run_limit = 0;		
	};

	using ptr = std::shared_ptr<task_runner>;

public: 
	task_runner(task_scheduler& scheduler, uint32_t id);

	~task_runner(); 

	bool start(const config& cfg);

	void push(task::ptr task);

	void finish();

	std::size_t get_task_count() const
	{
		return queue_.unsafe_size();
	}

private: 
	using task_queue = util::concurrent_queue<task::ptr>;

	void run();

private: 
	task_scheduler& scheduler_;
	uint32_t id_ = 0;
	config config_;
	task_queue queue_;
	std::atomic<bool> stop_ = true;
	std::thread thread_;

	unsigned int task_run_count_ = 0;
	float last_loop_time_ = 0.f;
	float average_loop_time_ = 0.f;	// moving average
	float total_loop_time_ = 0.f;
};

} // task
} // lax