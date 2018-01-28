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
	/// type�� ���ڿ��� �ּ� ��
	static constexpr type_t type = "component";
	static constexpr type_t type_none = "none_component";
	static constexpr type_t base_type = type_none;

	component(actor& owner);

	virtual ~component();

	/// ����. on_start() ȣ��. ���� �� false
	bool start();

	/// Ư�� runner���� ����. on_execute() ȣ��
	void execute();

	/// ����. on_finish() ȣ��. 
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