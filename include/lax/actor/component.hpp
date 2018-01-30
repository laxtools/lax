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

	/// 시작. on_start() 호출. 실패 시 false
	bool start();

	/// 특정 runner에서 실행. on_execute() 호출
	void execute();

	/// 종료. on_finish() 호출. 
	void finish();

	bool is_started() const
	{
		return started_;
	}

protected:
	/// 하위 클래스 구현. start()에서 호출
	virtual bool on_start();

	/// 하위 클래스 구현. execute()에서 호출
	virtual void on_execute();

	/// 하위 클래스 구현. finish()에서 호출
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