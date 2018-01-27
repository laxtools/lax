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
	/// ������
	task_adapter(actor::ptr target);

	/// �Ҹ���
	virtual ~task_adapter();

private:
	/// ���� Ŭ���� ����. start()���� ȣ��
	virtual bool on_start() override;

	/// ���� Ŭ���� ����. execute()���� ȣ��
	virtual void on_execute() override;

	/// ���� Ŭ���� ����. finish()���� ȣ��
	virtual void on_finish() override;

private:
	actor::ptr target_;
};

} // actor
} // lax