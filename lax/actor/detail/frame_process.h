#pragma once

#include <thread>

namespace lax
{
namespace actor
{

class frame_process
{
public: 
	frame_process() {}

	virtual ~frame_process() {}

	/// 처리 호출. runner만 쓰레드로 동작 
	/**
	* 가끔 실행되기 때문에 tick은 실행 후 흐른 시간
	*/
	void process(float tick);

	/// set run interval
	float set_schedule(float interval)
	{
		run_interval_ = std::max(interval, 0.f);

		return run_interval_;
	}

	float get_schedule() const
	{
		return run_interval_;
	}

	/// get curent run tick
	float get_current_tick() const
	{
		return current_tick_;
	}

protected:
	virtual void on_process() {}

private: 
	float				current_tick_ = 0.f;
	float				run_interval_ = 0.f;
};

inline void frame_process::process(float tick)
{
	if ((tick - current_tick_) > run_interval_)
	{
		current_tick_ = tick;

		on_process();
	}
}

} // actor
} // lax
