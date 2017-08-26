#include "stdafx.h"
#include "subscription.h"
#include <lax/actor/actor.h>

namespace lax
{
namespace actor
{

subscription::subscription()
	: seq_(1, UINT16_MAX)
{
}

subscription::~subscription()
{
	forget_all();
}

int subscription::remember(ptr target, int id)
{
	mem m; 

	m.target_ = target;
	m.id_ = id;
	m.mem_id_ = seq_.next();

	mems_[m.mem_id_] = m;

	return m.mem_id_;
}

bool subscription::forget(int id)
{
	auto iter = mems_.find(id);
	return_if(iter == mems_.end(), false);
	
	auto& m = iter->second;

	if (!m.target_.expired())
	{
		auto ptr = m.target_.lock();
		ptr->get_mailbox().unsubscribe(m.id_);
	}

	mems_.erase(iter);

	seq_.release(id);

	return true;
}

void subscription::forget_all()
{
	for (auto& iter : mems_)
	{
		auto& m = iter.second;

		if (!m.target_.expired())
		{
			auto ptr = m.target_.lock();
			ptr->get_mailbox().unsubscribe(m.id_);
		}
	}
}
	
} // actor
} // lax
