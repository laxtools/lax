#include "stdafx.h"

#include <lax/actor/cluster_actor.h>
#include <strstream>

namespace lax
{
namespace actor
{

cluster_actor::cluster_actor(ptr parent, id aid, ref::role role)
	: actor(parent, aid)
	, role_(role)
{
	setup_cluster_udpate_timer();
	setup_cluster_message_subscriptions();

	// 생성 통지 
}

cluster_actor::~cluster_actor()
{
	// 소멸 통지
}

directory& cluster_actor::get_dir_for_test_only()
{
	return dir_;
}

server* cluster_actor::get_server() const
{
	return nullptr; // temp
}

void cluster_actor::proc_state_self()
{
	// message 전송
}

void cluster_actor::on_cluster_state(net::msg::ptr m)
{
	
}

void cluster_actor::proc_attributes(
	id aid,
	ref::role role,
	ref::state astate,
	const json::Json& obj
)
{
}

void cluster_actor::add_attributes(json::Json& obj)
{

}

void cluster_actor::setup_cluster_udpate_timer()
{
	auto id = get_timer().set(10.f); // TODO: make time configurable

	// convert
	timer::action act = [this](int id) { id; this->proc_state_self();};

	get_timer().add(id, act);
}

void cluster_actor::setup_cluster_message_subscriptions()
{

}

void cluster_actor::build_desc()
{
	actor::build_desc();

	std::ostringstream  oss;

	oss << " self: cluster_actor." << " role: " << role_ << " base: " << desc_;

	desc_ = oss.str();
}

} // actor
} // lax
