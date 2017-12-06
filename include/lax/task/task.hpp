#pragma once 

#include <lax/util/macros.hpp>
#include <lax/util/simple_timer.hpp>
#include <atomic>
#include <memory>
#include <string>

namespace lax
{
namespace task
{

/// ���� �½�ũ Ŭ���� ���� �ϳ�. 
/** 
 * ���� �����. ����, �δ���, ��� ����. 
 *
 * ����: 
 *   - task�� ���� Ŭ������ ����. 
 *   - task_scheduler�� ��� 
 *   - �ʿ��� �� ��ü ����
 *   
 * ��å: 
 *   - affinity : �����Ǹ� ���� �����忡�� ����
 *   - round_robin : �����ٸ�. (������ �߰��� ���� ���) 
 */
class task : public std::enable_shared_from_this<task>
{
public: 
	using ptr = std::shared_ptr<task>;

	enum state
	{
		constructed,
		ready,
		suspended,
		finished,	
	};

public:
	explicit task(); 

	virtual ~task();

	bool start();

	void execute(uint32_t runner_id);

	void suspend();
	
	void resume();

	void finish();

	void set_affinity()
	{
		affinity_ = true;
	}

	bool is_affinity_set() const
	{
		return affinity_;
	}

	state get_state() const 
	{
		return state_;
	}

	uint32_t get_last_runner_id() const
	{
		return last_runner_id_;
	}

	float get_last_execution_time() const
	{
		return last_execution_time_;
	}

	unsigned int get_execution_count() const
	{
		return execution_count_;
	}

private:
	virtual bool on_start();

	virtual void on_execute();

	bool has_right_affinity(uint32_t runner_id) const;

private: 
	state state_ = state::constructed;
	std::atomic<bool> affinity_ = false;
	uint32_t last_runner_id_ = 0;

	util::simple_timer execution_timer_;
	unsigned int execution_count_;
	float last_execution_time_ = 0.f;
	float average_execution_time_ = 0.f;	// moving average
	float total_execution_time_ = 0.f;
};

} // task
} // lax