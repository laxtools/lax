#pragma once 

#include <lax/actor/cluster_actor.h>
#include <lax/util/simple_timer.h>

namespace lax
{
namespace actor
{

// a cluster_actor that has its own thread
class runner : public cluster_actor
{
public: 
	runner(ptr parent, id aid, ref::role role);
	~runner();

	/// 디버깅 등 위해
	int get_current_frame() const
	{
		return frame_;
	}

protected: 

	/// set sleep time for the current frame
	/** 
	 * on_process() 함수 호출에서 결정. 0이면 sleep 안 함
	 */
	void set_frame_sleep_time(std::size_t ms)
	{
		check(ms < 60 * 1000); // 1분 이상 sleep을 할 이유는 없다.

		frame_sleep_time_ = ms;
	}

private: 
	/// hide process function
	void process(float tick);

	/// 상위 process() 호출. 결국 on_process() 호출
	void run();

private:
	util::simple_timer timer_;
	std::thread thread_;
	std::atomic<bool> stop_ = false;
	std::size_t frame_sleep_time_ = 1; // 1ms default
	std::atomic<int> frame_ = 0;
};

} // actor 
} // lax

