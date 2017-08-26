#include "stdafx.h"

#include "actor.h"
#include <lax/util/logger.h>
#include <strstream>

namespace lax
{
namespace actor
{
actor::actor(id aid)
	: actor(nullptr, aid)
{
}

actor::actor(ptr parent, id aid) 
	: frame_process() 
	, parent_(parent)
	, id_(aid)
	, timer_(this)
	, mailbox_(this)
	, postbox_(this)
{
	build_desc();

	util::log()->trace("actor created. {0}", desc_);
}

actor::~actor()
{
	if (parent_)
	{
		parent_->remove_child(id_);
	}

	util::log()->trace("actor destroyed. {0}", desc_);
}

void actor::on_process()
{
	frame_process::on_process();

	// component
	for (auto& iter : comps_)
	{
		iter.second->process(get_current_tick());
	}

	// post messages
	post();

	// timer update
	timer_.update(get_current_tick());

	int run_count = 0;

	// run children
	for (auto& iter : children_)
	{
		iter.second->process(get_current_tick());

		run_count++;
	}
}

reason actor::send_direct(net::session::id sid, net::msg::ptr m)
{
	// not implemented
	sid; 
	m;

	return reason::fail_not_implemented;
}

reason actor::send(net::session::id sid, net::msg::ptr m)
{
	auto& binder = get_session_binder();

	if (binder.has(sid))
	{
		return binder.send(sid, m).value;
	}

	// 보모를 통해 전송
	if (get_parent())
	{
		return get_parent()->send(sid, m);
	}

	return send_direct(sid, m);
}

reason actor::send(id aid, net::msg::ptr m)
{
	auto& binder = get_session_binder();

	if (binder.has(aid))
	{
		return binder.send(aid, m).value;
	}

	// 보모를 통해 전송
	if (get_parent())
	{
		return get_parent()->send(aid, m);
	}

	return reason::fail_no_proper_sender;
}

int actor::send(const aid_vec& aids, net::msg::ptr m)
{
	int send_count = 0;

	for (auto& aid : aids)
	{
		auto rc = send(aid, m);

		if (rc) send_count++;
	}

	return send_count;
}

int actor::send(const sid_vec& sids, net::msg::ptr m)
{
	int send_count = 0;

	for (auto& sid : sids)
	{
		auto rc = send(sid, m);

		if (rc) send_count++;
	}

	return send_count;
}

void actor::push(net::msg::ptr m)
{
	auto pc = postbox_.post(m);

	if ( pc > 0 )
	{ 
		util::log()->trace(
			"actor. message posted directly. proto: {0} count: {1}", 
			m->get_protocol(), pc
		);
	}
	else		
	{
		queue_.push(m);
	}
}

void actor::post()
{
	net::msg::ptr m; 

	while (queue_.pop(m))
	{
		// TODO: make max frame count configurable
		// TODO: make slow handler log time 
		// TODO: make a state block and periodic log. 
		//		 make log interval configurable

		get_mailbox().post(m);
	}
}

bool actor::add_comp(component::ptr comp)
{
	check(!has_comp(comp->get_key()));

	return_if(has_comp(comp->get_key()), false);

	comps_.insert(component::map::value_type(comp->get_key(), comp));

	return comp->settle(this);
}

bool actor::has_child(actor::id id)
{
	auto iter = children_.find(id);

	return iter != children_.end();
}

void actor::add_child(actor::ptr child)
{
	check(child);
	check(child->get_id().is_valid());

	// to debug if already is there.
	auto iter = children_.find(child->get_id());
	check(iter == children_.end());

	children_[child->get_id()] = child;
}

bool actor::remove_child(actor::id id)
{
	auto iter = children_.find(id);
	return_if(iter == children_.end(), false);

	children_.erase(iter);

	return true;
}

int actor::subscribe_to(actor::ptr ptr, post::tag& tv, post::target& tgt)
{
	int id = ptr->get_mailbox().subscribe(tv, tgt);

	check(id > 0);

	std::weak_ptr<actor> wptr = ptr;

	return mailbox_subs_.remember(wptr, id);
}

int actor::subscribe_local(post::tag& tv, post::target& tgt)
{
	return subscribe_to(shared_from_this(), tv, tgt);
}

int actor::subscribe_post_to(actor::ptr ptr, post::tag& tv, post::target& tgt)
{
	int id = ptr->get_postbox().subscribe(tv, tgt);

	check(id > 0);

	std::weak_ptr<actor> wptr = ptr;

	return postbox_subs_.remember(wptr, id);
}

int actor::subscribe_push(actor::ptr ptr, post::tag& tv)
{
	post::target tgt; 

	tgt.exec_ = post::target::exec( [this](net::msg::ptr m) 
	{
		this->push(m); 
	});

	return subscribe_post_to(ptr, tv, tgt);
}

void actor::build_desc()
{
	std::ostringstream  oss; 

	oss << " actor: " << id_.value;
	oss << " parent: ";

	if (parent_)
	{
		oss << parent_->get_id().value;
	}
	else
	{
		oss << 0;
	}

	desc_ = oss.str();
}

} // actor 
} // lax
