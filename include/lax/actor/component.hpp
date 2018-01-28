#pragma once
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
class component 
{
public:
	using ptr = std::shared_ptr<component>;
	using type_t = const char *;

public:
	/// type은 문자열의 주소 값
	static constexpr type_t type = "component";
	static constexpr type_t type_none = "none_component";
	static constexpr type_t base_type = type_none;

	component(actor& owner);

	virtual ~component();

	/// 시작. on_start() 호출. 실패 시 false
	bool start();

	/// 특정 runner에서 실행. on_execute() 호출
	void execute();

	/// 종료. on_finish() 호출. 
	void finish();

	type_t get_type() const
	{
		return type;
	}

	type_t get_base_type() const
	{
		return base_type;
	}

	bool is_a(type_t type) const;

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

protected:
	std::vector<type_t> types_;

private: 
	actor& owner_;
};

} // actor
} // lax

#define COMPONENT_HEAD(cls) \
static constexpr type_t type = #cls; \
static constexpr type_t base_type = type_none; \
using base_class = lax::actor::component; \
using ptr = std::shared_ptr<cls>; \
type_t get_type() const { return type; } \
type_t get_base_type() const { return base_type; } \
cls(actor& owner) : component(owner) { types_.push_back(get_type()); }

#define COMPONENT_HEAD_INH(cls, base) \
static constexpr type_t type = #cls; \
static constexpr type_t base_type = base::type; \
using base_class = base; \
using ptr = std::shared_ptr<cls>; \
type_t get_type() const { return type; } \
type_t get_base_type() const { return base_type; } \
cls(actor& owner) : base(owner) { types_.push_back(get_type()); }