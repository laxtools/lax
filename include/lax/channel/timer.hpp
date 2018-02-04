#pragma once 

#include <lax/util/macros.hpp>
#include <lax/util/sequence.hpp>
#include <lax/util/result.hpp>
#include <lax/util/simple_timer.hpp>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>

namespace lax
{
namespace channel
{

/// timer that checks efficiently
/** 
 * interval based timer 
 * 
 * Usage: 
 *   auto id = get_timer().set( 10, .... )    // every 10 sec
 *   get_timer().add( id, on_update_state )   // set callback
 *
 *   on_update_state( id )
 *   {
 *      send_to_servers( state )
 *   }
 * 
 * Performance:
 * - less than O(nlogn) where n is the reqs in a slot
 */
class timer
{
public: 

	using id_t = int;
	using tick_t = float;
	using action = std::function<void(id_t)>;
	using actions = std::vector<action>;

	static constexpr tick_t forever = 0.f;
	static constexpr tick_t min_interval = 0.01f;

public:
	/// constructor
	timer();

	/// check slots to run 
	void update();

	/// sets interval
	id_t set(
		tick_t interval,
		tick_t duration = forever,
		bool repeat = true,
		tick_t after = 0.f);

	/// add callback action to id req 
	bool add(id_t id, action act);
	
	/// cancel the req (remove)
	bool cancel(id_t id);

	/// VERIFY 
	bool has(id_t id) const;

	/// get last run tick
	tick_t get_last_run_tick(id_t id) const;

	/// get next scheduled tick
	tick_t get_next_run_tick(id_t id) const;

	/// test purpose
	std::size_t get_slot_count() const 
	{ 
		return slots_.size(); 
	}

	/// test purpose
	std::size_t get_req_count() const
	{
		return reqs_.size();
	}

	/// test purpose
	std::size_t get_run_count(id_t id) const;

private:
	struct req
	{
		using ptr = std::shared_ptr<req>;

		req() = default;
			
		req(timer& t,
			id_t id, 
			tick_t interval, tick_t duration, tick_t after, bool repeat);

		~req()
		{
			release();
		}

		/// add callback actions
		req& add(action& act)
		{
			actions_.push_back(act);
			return *this;
		}

		/// call back 
		void run();

		/// release timer id
		void release();

		/// disable copy && assignment
		req(const req& rhs) = delete;
		req& operator=(req& rhs) = delete;

		// members.

		timer& timer_;
		id_t id_ = 0;
		tick_t interval_ = 0.f;
		tick_t duration_ = forever;
		tick_t after_ = 0.f;
		bool repeat_ = true;
		actions actions_;

		tick_t last_run_tick_ = 0.f;
		tick_t next_run_tick_ = 0.f;
		tick_t end_run_tick_ = 0.f;

		bool cancel_ = false;
		std::size_t run_count_ = 0;
	};

	struct slot
	{
		struct entry
		{
			req::ptr ptr_;

			entry(req::ptr p)
				: ptr_(p)
			{
			}

			bool operator < (const entry& rhs) const
			{
				return ptr_->next_run_tick_ < rhs.ptr_->next_run_tick_;
			}
		};

		tick_t interval_ = 0.f;
		tick_t last_run_tick_ = 0.f;
		std::priority_queue<entry> reqs_;
	};

	using slots = std::vector<slot>;
	using reqs = std::unordered_map<id_t, req::ptr>;

private: 
	/// 적절한 interval 간격 슬롯이 있는 지 찾고 없으면 생성
	slot& create_slot(tick_t interval);

	/// 슬롯 타이머 처리
	void update_slot(slot& slt, tick_t tick);

	/// remove req
	void remove_end_req(id_t id);

	tick_t get_current_tick() const
	{
		return timer_.elapsed();
	}

private:
	util::simple_timer timer_;
	slots slots_;
	reqs reqs_;
	util::sequence<uint32_t> seq_; // timer id sequence
};


inline void timer::update()
{
	auto tick = get_current_tick();

	for (auto& sl : slots_)
	{
		update_slot(sl, tick);
	}
}

inline void timer::update_slot(slot& sl, tick_t now)
{
	VERIFY(sl.interval_ > min_interval);

	if ((sl.last_run_tick_ + sl.interval_) > now )
	{
		return;
	}

	sl.last_run_tick_ = now;

	RETURN_IF(sl.reqs_.empty());

	auto e = sl.reqs_.top();

	while (e.ptr_->next_run_tick_ < now)
	{
		sl.reqs_.pop();

		if (!e.ptr_->cancel_)
		{
			e.ptr_->last_run_tick_ = now;

			e.ptr_->run();

			if (e.ptr_->repeat_ || now < e.ptr_->end_run_tick_)
			{
				e.ptr_->next_run_tick_ = now + e.ptr_->interval_;

				sl.reqs_.push(e);
			}
			else
			{
				remove_end_req(e.ptr_->id_);
			}
		}

		BREAK_IF(sl.reqs_.empty());

		e = sl.reqs_.top();
	}
}

} // channel
} // lax
