#include "stdafx.h"
#include <lax/channel/timer.hpp>

namespace lax
{
namespace channel
{

timer::timer()
	: seq_(1, UINT32_MAX, 16)
{
}

int timer::set(tick_t interval, tick_t duration, bool repeat, tick_t after)
{
	auto si = min_interval;
	auto fi = si;
	auto target_interval = interval / 2; // ������ �Ǿ�� Ÿ�̸� ó�� �л��� ��

	while (si < target_interval)
	{
		fi = si;
		si *= 2;
	}

	VERIFY(fi <= interval);
	VERIFY(fi <= target_interval);

	auto& slt = create_slot(fi);

	auto rp = std::make_shared<req>(*this, seq_.next(), interval, duration, after, repeat);

	reqs_.emplace(rp->id_, rp);

	slt.reqs_.push(slot::entry(reqs_[rp->id_]));

	return rp->id_;
}

bool timer::has(id_t id) const
{
	return reqs_.find(id) != reqs_.end();
}

bool timer::add(id_t id, action act)
{
	auto iter = reqs_.find(id);

	RETURN_IF(iter == reqs_.end(), false);

	iter->second->add(act);

	return true;
}

bool timer::cancel(id_t id)
{
	auto iter = reqs_.find(id);

	RETURN_IF(iter == reqs_.end(), false);

	iter->second->cancel_ = true;	// ��ҵ� ��û���� slot ������ �� ����

	// safe to erase 
	reqs_.erase(iter);

	return true;
}

timer::tick_t timer::get_next_run_tick(id_t id) const
{
	auto iter = reqs_.find(id);

	RETURN_IF(iter == reqs_.end(), 0.f);

	return iter->second->next_run_tick_;
}

timer::tick_t timer::get_last_run_tick(id_t id) const
{
	auto iter = reqs_.find(id);

	RETURN_IF(iter == reqs_.end(), 0.f);

	return iter->second->last_run_tick_;
}

std::size_t timer::get_run_count(id_t id) const
{
	auto iter = reqs_.find(id);

	RETURN_IF(iter == reqs_.end(), 0);

	return iter->second->run_count_;
}

timer::slot& timer::create_slot(tick_t interval)
{
	VERIFY(interval >= min_interval);

	// interval ���� ���� ������ ���Ե� �� �ִ� ������ ã�´�. 
	// ������ ���ĵǾ� ���� �ʴ�.

	slot* found = nullptr;
	tick_t found_interval = 0;

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

	// ���ų� �ش� ������ ������ interval�� ���� ���� ũ�� �߰� �����Ѵ�. 
	if (found == nullptr || found->interval_ > interval )
	{
		slot slt;
		
		slt.interval_ = interval;
		slt.last_run_tick_ = 0.f;

		slots_.push_back(slt);

		return *slots_.rbegin();
	}

	VERIFY(found);

	return *found;
}

void timer::remove_end_req(id_t id)
{
	auto iter = reqs_.find(id);

	RETURN_IF(iter == reqs_.end());

	reqs_.erase(iter);
}

timer::req::req(timer& t,
	id_t id, 
	tick_t interval, tick_t duration, tick_t after, bool repeat)
	: timer_(t)
	, id_(id)
	, interval_(interval)
	, duration_(duration)
	, after_(after)
	, repeat_(repeat)
{
	VERIFY(id_ > 0);
	VERIFY(interval_ > 0);
	VERIFY(duration_ >= 0);
	VERIFY(after_ >= 0);

	interval_ = std::max(min_interval, interval_);			// clamp

	next_run_tick_ = t.get_current_tick() + after_ + interval_;
	end_run_tick_ = t.get_current_tick() + after_ + duration_;
}

void timer::req::run()
{
	VERIFY(id_ > 0);

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

	id_ = 0;
}


} // channel
} // lax
