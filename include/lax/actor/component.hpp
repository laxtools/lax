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

	/// ����. on_start() ȣ��. ���� �� false
	bool start();

	/// Ư�� runner���� ����. on_execute() ȣ��
	void execute();

	/// ����. on_finish() ȣ��. 
	void finish();

	virtual type_t get_type() const
	{
		return "component";
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
