#pragma once 

#include <lax/util/macros.h>
#include <lax/util/sequence.h>
#include <lax/util/result.h>
#include <functional>
#include <vector>
#include <queue>

namespace lax
{
namespace actor
{

class actor;

/// actor timer that checks efficiently
/** 
 * interval based timer for actor
 * 
 * ���: 
 *   - ������ �������� Ÿ�̸ӵ� ���� 
 *   - �켱���� ť�� ���� ���� ����Ǿ�� �ϴ� ������ ����
 * 
 * Usage: 
 *   auto id = get_timer().set( 10, .... )    // 10�ʸ��� ����
 *   get_timer().add( id, on_update_state )   // ���� �Լ� �߰�
 *
 *   on_update_state( id )
 *   {
 *      send_to_servers( state )
 *   }
 *
 * Ư��: 
 *  - update ȣ�⿡�� tick�� ���� ���� �ѹ��� ���� 
 *  - �ݹ� ȣ���� ���� �ɷ��� �ѹ��� ���� (���� ���� ����)
 *  - ��û�� ������ �������� üũ ����. (ȣ�� �л�)
 *  - �ݹ� �Լ����� Ÿ�̸� �߰��� ��� ����
 */
class timer
{
public: 
	static const float forever;

	using action = std::function<void(int)>;
	using actions = std::vector<action>;

public:
	/// constructor
	timer(actor* owner);

	/// check slots to run 
	void update(float tick);

	/// sets interval
	int set(
		float interval,
		float duration = forever,
		bool repeat = true,
		float after = 0.f);

	/// check 
	bool has(int id) const;

	/// add callback action to id req 
	bool add(int id, action& act);
	
	/// cancel the req
	bool cancel(int id);

	/// get last run tick
	float get_last_run_tick(int id) const;

	/// get next scheduled tick
	float get_next_run_tick(int id) const;

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
	std::size_t get_run_count(int id) const;

private:
	struct req
	{
		using ptr = std::shared_ptr<req>;

		req() = default;
			
		req(timer& t,
			int id, 
			float interval, float duration, float after, bool repeat);

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
		int id_ = 0;
		float interval_ = 0.f;
		float duration_ = forever;
		float after_ = 0.f;
		bool repeat_ = true;
		actions actions_;

		float last_run_tick_ = 0.f;
		float next_run_tick_ = 0.f;
		float end_run_tick_ = 0.f;

		bool cancel_ = false;
		int run_count_ = 0;
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

		float interval_ = 0.f;
		float last_run_tick_ = 0.f;
		std::priority_queue<entry> reqs_;
	};

	using slots = std::vector<slot>;
	using reqs = std::unordered_map<int, req::ptr>;

private: 
	static const float min_interval; 

	/// ������ interval ���� ������ �ִ� �� ã�� ������ ����
	slot& create_slot(float interval);

	/// ���� Ÿ�̸� ó��
	void update_slot(slot& slt, float tick);

	/// remove req
	void remove_end_req(int id);

private:
	actor* owner_ = nullptr;
	slots slots_;
	reqs reqs_;
	util::sequence<uint32_t> seq_; // timer id ������
};


inline void timer::update(float tick)
{
	for (auto& sl : slots_)
	{
		update_slot(sl, tick);
	}
}

inline void timer::update_slot(slot& sl, float now)
{
	check(sl.interval_ > min_interval);

	if ((sl.last_run_tick_ + sl.interval_) > now )
	{
		return;
	}

	sl.last_run_tick_ = now;

	return_if(sl.reqs_.empty());

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

		break_if(sl.reqs_.empty());

		e = sl.reqs_.top();
	}
}

} // actor 
} // lax
