#include "stdafx.h"
#include "timer.h"
#include <lax/actor/actor.h>

namespace lax
{
namespace actor
{

const float timer::forever = 0.f;
const float timer::min_interval = 0.01f; 

timer::timer(actor* owner)
	: seq_(1, UINT32_MAX, 16)
	, owner_(owner)
{
	check(owner_);

	if (!owner_)
	{
		throw std::exception("timer must have a valid owner");
	}
}

int timer::set(float interval, float duration, bool repeat, float after)
{
	auto si = min_interval;
	auto fi = si;
	auto target_interval = interval / 2; // 절반이 되어야 타이머 처리 분산이 됨

	while (si < target_interval)
	{
		fi = si;
		si *= 2;
	}

	check(fi <= interval);
	check(fi <= target_interval);

	auto& slt = create_slot(fi);

	auto rp = std::make_shared<req>(*this, seq_.next(), interval, duration, after, repeat);

	reqs_.emplace(rp->id_, rp);

	slt.reqs_.push(slot::entry(reqs_[rp->id_]));

	return rp->id_;
}

bool timer::has(int id) const
{
	return reqs_.find(id) != reqs_.end();
}

bool timer::add(int id, action& act)
{
	auto iter = reqs_.find(id);

	return_if(iter == reqs_.end(), false);

	iter->second->add(act);

	return true;
}

bool timer::cancel(int id)
{
	auto iter = reqs_.find(id);

	return_if(iter == reqs_.end(), false);

	iter->second->cancel_ = true;	// 취소된 요청들은 slot 실행할 때 제거

	// safe to erase 
	reqs_.erase(iter);

	return true;
}

float timer::get_next_run_tick(int id) const
{
	auto iter = reqs_.find(id);

	return_if(iter == reqs_.end(), 0.f);

	return iter->second->next_run_tick_;
}

float timer::get_last_run_tick(int id) const
{
	auto iter = reqs_.find(id);

	return_if(iter == reqs_.end(), 0.f);

	return iter->second->last_run_tick_;
}

std::size_t timer::get_run_count(int id) const
{
	auto iter = reqs_.find(id);

	return_if(iter == reqs_.end(), 0);

	return iter->second->run_count_;
}

timer::slot& timer::create_slot(float interval)
{
	check(interval >= min_interval);

	// interval 보다 작은 간격의 슬롯들 중 최대 슬롯을 찾는다. 
	// 슬롯은 정렬되어 있지 않다.

	slot* found = nullptr;
	float found_interval = 0;

	for (auto& slt : slots_)
	{
		if (slt.interval_ < interval)
		{
			if (found_interval < slt.interval_)
			{
				found = &slt;
				found_interval = slt.interval_;
			}
		}
	}

	// 없거나 해당 슬록의 간격이 interval의 절반 보다 크면 추가 생성한다. 
	if (found == nullptr || found->interval_ > interval )
	{
		slot slt;
		
		slt.interval_ = interval;
		slt.last_run_tick_ = 0.f;

		slots_.push_back(slt);

		return *slots_.rbegin();
	}

	check(found);

	return *found;
}

void timer::remove_end_req(int id)
{
	auto iter = reqs_.find(id);

	return_if(iter == reqs_.end());

	reqs_.erase(iter);
}

timer::req::req(timer& t,
	int id, 
	float interval, float duration, float after, bool repeat)
	: timer_(t)
	, id_(id)
	, interval_(interval)
	, duration_(duration)
	, after_(after)
	, repeat_(repeat)
{
	check(id_ > 0);
	check(interval_ > 0);
	check(duration_ >= 0);
	check(after_ >= 0);

	interval_ = std::max(min_interval, interval_);			// clamp

	next_run_tick_ = timer_.owner_->get_current_tick() + after_ + interval_;
	end_run_tick_ = timer_.owner_->get_current_tick() + after_ + duration_;
}

void timer::req::run()
{
	check(id_ > 0);

	for (auto& act : actions_)
	{
		act(id_);
	}

	++run_count_;
}

void timer::req::release()
{
	if (id_ > 0)
	{
		timer_.seq_.release(id_);
	}
}


} // actor 
} // lax
