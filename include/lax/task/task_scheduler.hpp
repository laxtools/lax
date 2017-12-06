#pragma once 

#include <lax/util/concurrent_queue.hpp>
#include <lax/task/task.hpp>
#include <lax/task/detail/task_runner.hpp>
#include <vector>

namespace lax
{
namespace task
{

class task_scheduler
{
public: 
	enum policy
	{
		// 이걸로 하는 건 좀 그렇네.... 예상 시간으로 하는게 맞지 않을까?
		min_task_count,			

		min_task_expected_run_time, 			
	};

	struct config
	{
		/// runner 개수
		int runner_count = 1;

		/// 기본 값 1ms. 이보다 짧게 걸리면 1ms 쉬기
		float idle_check_threshold_time = 0.001f;

		/// 0 : 무한. 숫자 : 개수 제한
		unsigned int single_loop_run_limit = 0;

		/// schedule에서 시작 안 된 태스크는 시작시킴
		bool start_task_when_schedule_called = true;

		/// schedule() 호출 시 스케줄 할 지 여부
		bool run_schedule_when_schedule_called = false;

		/// finish 중에 ready인 태스크들을 종료 시킴 
		bool finish_ready_task_during_finish = true;
	};

public: 
	task_scheduler(); 

	~task_scheduler();

	bool start(const config& cfg);

	void schedule(task::ptr task);

	void run();

	void finish();

	std::size_t get_runner_count() const
	{
		return runners_.size();
	}

	/// 전체 태스크 개수 (thread unsafe)
	std::size_t get_task_count() const;

private: 
	using task_queue = util::concurrent_queue<task::ptr>;
	using runners = std::vector<task_runner::ptr>;

	void run_schedule();

	uint32_t get_target_runner_id() const;

private:
	config config_;
	task_queue queue_;
	runners runners_;
};

} // task
} // lax