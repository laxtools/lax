#include "stdafx.h"
#include <lax/actor/component.hpp>

namespace lax 
{
namespace actor 
{

component::component(actor& owner)
	: owner_(owner)
{
	push_type<component>();
}

component::~component()
{
}

bool component::start()
{
	started_ = on_start();

	return started_;
}

void component::execute()
{
	on_execute();
}

void component::finish()
{
	on_finish();

	started_ = false;
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
