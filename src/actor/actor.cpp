#include "stdafx.h"
#include <lax/actor/actor.hpp>
#include <basetsd.h>

namespace lax 
{
namespace actor 
{

util::sequence<uint32_t, std::recursive_mutex> actor::id_seq_(1, MAXUINT32, 1024);

actor::actor(weak_ptr parent)
	: parent_(parent)
	, id_(id_seq_.next())
{
}

actor::~actor()
{
	id_seq_.release(id_);
}

bool actor::start()
{
	auto rc = on_start();
	return_if(!rc, false);

	for (auto& iter : comps_)
	{
		rc = rc && iter.second->start();
	}

	started_ = rc;

	return rc;
}

void actor::execute()
{
	on_execute();
}

void actor::finish()
{
	on_finish();

	started_ = false;
}

bool actor::on_start()
{
	return true;
}

void actor::on_execute()
{
}

void actor::on_finish()
{
}

bool actor::add_comp(component::ptr comp)
{
	comps_[comp->type] = comp;

	return !!comp;
}

component::ptr actor::get_comp(component::type_t type) const
{
	auto iter = comps_.find(type);
	return_if(iter == comps_.end(), component::ptr());

	return iter->second;
}

} // actor 
} // lax
