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

/// 많은 태스크 클래스 중의 하나. 
/** 
 * 게임 실행기. 지역, 인던들, 방들 실행. 
 *
 * 사용법: 
 *   - task의 하위 클래스를 만듦. 
 *   - task_scheduler에 등록 
 *   - 필요할 때 자체 종료
 *   
 * 정책: 
 *   - affinity : 지정되면 동일 쓰레드에서 실행
 *   - round_robin : 스케줄링. (나중의 추가를 위해 사용) 
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