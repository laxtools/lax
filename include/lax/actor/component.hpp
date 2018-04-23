#pragma once
#include <lax/actor/detail/type_object.hpp>
#include <lax/util/macros.hpp>
#include <memory>
#include <stdint.h>
#include <vector>

namespace lax
{
namespace actor
{

class actor;

/// componet base for actor to separate concerns
/**
 */
class component  : public type_object
{
public:
	using ptr = std::shared_ptr<component>;

public:
	component(actor& owner);

	virtual ~component();

	/// ����. on_start() ȣ��. ���� �� false
	bool start();

	/// Ư�� runner���� ����. on_execute() ȣ��
	void execute();

	/// ����. on_finish() ȣ��. 
	void finish();

	bool is_started() const
	{
		return started_;
	}

protected:
	/// ���� Ŭ���� ����. start()���� ȣ��
	virtual bool on_start();

	/// ���� Ŭ���� ����. execute()���� ȣ��
	virtual void on_execute();

	/// ���� Ŭ���� ����. finish()���� ȣ��
	virtual void on_finish();

	const actor& get_owner() const
	{
		return owner_;
	}

	actor& get_owner()
	{
		return owner_;
	}

private: 
	actor& owner_;
	bool started_ = false;
};

} // actor
} // lax