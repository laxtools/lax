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
		// �̰ɷ� �ϴ� �� �� �׷���.... ���� �ð����� �ϴ°� ���� ������?
		min_task_count,			

		min_task_expected_run_time, 			
	};

	struct config
	{
		/// runner ����
		int runner_count = 1;

		/// �⺻ �� 1ms. �̺��� ª�� �ɸ��� 1ms ����
		float idle_check_threshold_time = 0.001f;

		/// 0 : ����. ���� : ���� ����
		unsigned int single_loop_run_limit = 0;

		/// schedule���� ���� �� �� �½�ũ�� ���۽�Ŵ
		bool start_task_when_schedule_called = true;

		/// schedule() ȣ�� �� ������ �� �� ����
		bool run_schedule_when_schedule_called = false;

		/// finish �߿� ready�� �½�ũ���� ���� ��Ŵ 
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

	/// ��ü �½�ũ ���� (thread unsafe)
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