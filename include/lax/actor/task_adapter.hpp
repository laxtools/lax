#pragma once

#include <lax/actor/actor.hpp>
#include <lax/task/task.hpp>

namespace lax
{
namespace actor
{

class task_adapter : public task::task
{
public:
	/// 생성자
	task_adapter(actor::ptr target);

	/// 소멸자
	virtual ~task_adapter();

private:
	/// 하위 클래스 구현. start()에서 호출
	virtual bool on_start() override;

	/// 하위 클래스 구현. execute()에서 호출
	virtual void on_execute() override;

	/// 하위 클래스 구현. finish()에서 호출
	virtual void on_finish() override;

private:
	actor::ptr target_;
};

} // actor
} // lax