#include "stdafx.h"

#include "mailbox.h"
#include <lax/actor/actor.h>
#include <lax/net/protocol/protocol.h>
#include <lax/net/protocol/json/msg_json.h>
#include <lax/net/protocol/msgpack/msg_pack.h>
#include <lax/net/protocol/local/msg_local.h>
#include <lax/util/logger.h>

#include <functional>
namespace lax
{
namespace actor
{

mailbox::mailbox(actor* owner)
	: owner_(owner)
{
}

mailbox::~mailbox()
{
}

int mailbox::subscribe(tag_binder::tag tv, tag_binder::target& tgt)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);
	return subs_.sub(tv, tgt);
}

bool mailbox::unsubscribe(int id)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);
	return subs_.unsub(id);
}

int mailbox::unsubscribe(std::vector<int>& ids)
{
	// NOTE: don't lock
	int unsub_count = 0;

	for (auto& id : ids)
	{
		unsubscribe(id) ? ++unsub_count : 0;
	}

	return unsub_count;
}

int mailbox::post(net::msg::ptr m) const
{
	std::shared_lock<std::shared_timed_mutex> lock(mutex_);
	return subs_.post(m);
}

mailbox::subs::subs()
	: seq_(1, UINT16_MAX)
{
}

int mailbox::subs::sub(tag_binder::tag& atag, tag_binder::target& to)
{
	to.id_ = seq_.next();

	auto iter = subs_.find(atag);

	if (iter == subs_.end())
	{
		targets lst;

		lst.push_back(to);

		auto rc = subs_.insert(map::value_type(atag, lst));
	}
	else
	{
		iter->second.push_back(to);
	}

	index_[to.id_] = atag;

	return to.id_;
}

int mailbox::subs::post(net::msg::ptr m) const
{
	int tag_bound_count = 0;

	// tag_binder with key
	{
		tag_binder::tag tv;

		if (build_tag_with_key(tv, m))
		{
			if (tv.key_type_ != tag_binder::tag::invalid)
			{
				tag_bound_count += tag_binder_by_tag(tv, m);
			}
		}
	}

	// tag_binder with type
	{
		tag_binder::tag tv;

		if (build_tag_with_type(tv, m))
		{
			if (tv.key_type_ != tag_binder::tag::invalid)
			{
				tag_bound_count += tag_binder_by_tag(tv, m);
			}
		}
	}

	return tag_bound_count;
}

bool mailbox::subs::unsub(int id)
{
	auto iter = index_.find(id);

	return_if(iter == index_.end(), false);

	auto ti = subs_.find(iter->second);

	return_if(ti == subs_.end(), false);

	auto& vec = ti->second;

	for (auto& to : vec)
	{
		if (to.id_ == id)
		{
			seq_.release(id);
			break;
		}
	}

	vec.erase(
		std::remove_if(
			vec.begin(), vec.end(), 
			[id](const tag_binder::target& tg) { return tg.id_ == id; }
		),
		vec.end()
	);

	if (vec.empty())
	{
		subs_.erase(ti);
	}

	return true;
}

int mailbox::subs::tag_binder_by_tag(tag_binder::tag& tv, net::msg::ptr m) const
{
	// find 
	auto ti = subs_.find(tv);

	if (ti == subs_.end())
	{
		if (tv.key_type_ == tag_binder::tag::key_type::string)
		{
			util::log()->warn(
				"mailbox. tag not found for proto:{0} key:{1}", tv.proto_, tv.s_key_
			);
		}
		else
		{
			util::log()->warn(
				"mailbox. tag not found for proto:{0} key:{1}", tv.proto_, tv.v_key_
			);
		}

		return 0;
	}

	// execute (callback)

	auto& vec = ti->second;

	int tag_bound_count = 0;

	for (auto& tg : vec)
	{
		if (tg.exec_)
		{
			if (tg.cond_)
			{
				if (tg.cond_(m, tg.value_))
				{
					tg.exec_(m);
					++tag_bound_count;
				}
			}
			else
			{
				tg.exec_(m);
				++tag_bound_count;
			}
		}
		else
		{
			// log. exception?
		}
	}

	if (tag_bound_count == 0)
	{
		if (tv.key_type_ == tag_binder::tag::key_type::string)
		{
			util::log()->warn(
				"mailbox. not processed for proto:{0} key:{1}", tv.proto_, tv.s_key_
			);
		}
		else
		{
			util::log()->warn(
				"mailbox. not processed for proto:{0} key:{1}", tv.proto_, tv.v_key_
			);
		}
	}

	return tag_bound_count;
}

bool mailbox::subs::build_tag_with_key(tag_binder::tag& tv, net::msg::ptr m) const
{
	switch (m->get_protocol())
	{
	case net::protocol::msgpack:
	{
		auto mf = std::static_pointer_cast<net::msg_pack>(m);
		tv.set(m->get_protocol(), mf->get_key());
		return true;
	}
	break;
	case net::protocol::local:
	{
		auto mf = std::static_pointer_cast<net::msg_local>(m);
		tv.set(m->get_protocol(), mf->get_key());
		return true;
	}
	break;
	case net::protocol::json:
	{
		auto mf = std::static_pointer_cast<net::msg_json>(m);
		tv.set(m->get_protocol(), mf->get_key());
		return true;
	}
	break;
	}

	return false;
}

bool mailbox::subs::build_tag_with_type(tag_binder::tag& tv, net::msg::ptr m) const
{
	switch (m->get_protocol())
	{
	case net::protocol::msgpack:
	{
		auto mf = std::static_pointer_cast<net::msg_pack>(m);
		tv.set(m->get_protocol(), mf->get_type());
		return true;
	}
	}

	return false;
}

} // actor 
}  // lax
