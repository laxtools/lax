#include "stdafx.h"
#include <lax/actor/component.hpp>

namespace lax 
{
namespace actor 
{

component::component(actor& owner)
	: owner_(owner)
{
}

component::~component()
{
}

bool component::start()
{
	return on_start();
}

void component::execute()
{
	on_execute();
}

void component::finish()
{
	on_finish();
}

bool component::is_a(type_t type) const
{
	for (auto& stype : types_)
	{
		if (stype == type)
		{
			return true;
		}
	}

	return false;
}

bool component::on_start()
{
	return true;
}

void component::on_execute()
{
}

void component::on_finish()
{
}


} // component 
} // lax
