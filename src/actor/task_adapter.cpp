#include "stdafx.h"
#include <lax/actor/task_adapter.hpp>

namespace lax
{
namespace actor
{

task_adapter::task_adapter(actor::ptr target)
	: task()
	, target_(target)
{
}

task_adapter::~task_adapter()
{
}

bool task_adapter::on_start()
{
	return target_->start();
}

void task_adapter::on_execute()
{
	target_->execute();
}

void task_adapter::on_finish()
{
	target_->finish();
}

} // actor
} // lax