#pragma once
#include <lax/util/macros.hpp>
#include <memory>
#include <stdint.h>

namespace lax
{
namespace actor
{

class actor;

/// componet abstract base for actor to separate concerns
/**
 */
class component 
{
public:
	using ptr = std::shared_ptr<component>;
	using type_t = const char *;

public:
	static constexpr type_t type = "component";

	component(actor& owner);

	virtual ~component();

	/// 시작. on_start() 호출. 실패 시 false
	bool start();

	/// 특정 runner에서 실행. on_execute() 호출
	void execute();

	/// 종료. on_finish() 호출. 
	void finish();

	virtual type_t get_type() const
	{
		return "component";
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
};

} // actor
} // lax

#define COMPONENT_HEAD(cls) \
static constexpr type_t type = #cls; \
using ptr = std::shared_ptr<cls>; \
type_t get_type() const override { return type; } \
cls get_owner() { return get_owner(); } \
const cls get_owner() const { return get_owner(); } \
cls(actor& owner) : component(owner) {}
