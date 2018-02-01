#include "stdafx.h"
#include <lax/actor/task_adaptor.hpp>

namespace lax
{
namespace actor
{

task_adaptor::task_adaptor(actor::ptr target)
	: task()
	, target_(target)
{
	set_desc("task_adaptor");
}

task_adaptor::~task_adaptor()
{
}

bool task_adaptor::on_start()
{
	return target_->start();
}

void task_adaptor::on_execute()
{
	target_->execute();
}

void task_adaptor::on_finish()
{
	target_->finish();
}

} // actor
} // lax