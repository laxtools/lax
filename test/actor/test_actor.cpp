#include "stdafx.h"
#include <catch.hpp>
#include <lax/actor/actor.hpp>
#include <lax/actor/task_adapter.hpp>

using namespace lax::actor;

namespace
{

class test_actor : public actor
{
public:
	ACTOR_HEAD(test_actor);

private:
	virtual bool on_start() override
	{
		return true;
	}

	virtual void on_execute() override
	{

	}

	virtual void on_finish() override
	{

	}
};

class movement_comp : public component
{
public:
	COMPONENT_HEAD(movement_comp); // name, ptr, get_owner

	void move()
	{
	}
};

} // noname

TEST_CASE("test actor")
{
	// ���� ������ ����鼭 �ʿ��� ����� Ȯ���Ѵ�.
	// �� ���� ��Ģ�� �����Ѵ�.
	// - type safety�� �ִ��� �����ϸ鼭 �������� �����Ѵ�.
	// - ������Ʈ�� �ʿ� ������ actor ���� ó���� �ϰ��Ѵ�.
	//   - ������� �ʴ� ������Ʈ�� ��� ���� �ǹ̸� �ִ�.
	//   

	SECTION("basic interface")
	{
		auto ap = std::make_shared<test_actor>();

		REQUIRE(ap->start());

		REQUIRE(ap->get_type() == test_actor::type);

		ap->execute();

		ap->finish();
	}

	SECTION("task actor")
	{
		auto ap = std::make_shared<test_actor>();
		auto tp = std::make_shared<task_adapter>(ap);

		REQUIRE(tp->start());
		REQUIRE(ap->is_started());
		tp->execute(0);
		tp->finish();
		REQUIRE(!ap->is_started());

		//
		// actor�� task_scheduler�� ����� �� ���
		//
	}

	SECTION("component")
	{
		auto ap = std::make_shared<test_actor>();

		ap->add_comp<movement_comp>();

		auto movement = ap->get_comp<movement_comp>();

		if (movement)
		{
			movement->move();
		}

		//
		// ������Ʈ�� ���: 
		// c++���� �߸� �������� ������ ������Ʈ�� ������� �ʴ� �� ������ ����.
		// �׳� ��� �Լ��� ���� Ŭ������ �ۼ��ϴ� ���� �� ���� �� �ִ�. 
		// ����, ������ ���� �����ϰ� ���� ������ ������. 
		// - actor�� �����ڿ����� Ư���� ��찡 �ƴ϶�� add_comp<Comp>()�� 
		//   ��� ����Ѵ�. 
		// - ����� ���� get_comp<Comp>()�� �ϰ� ������ üũ �� ����Ѵ�. 
		// - actor�鿡 ������ ���� actor::for_each<Comp>�� ����Ѵ�.
		//
		// ������Ʈ�� ����:
		// COMPONENT_HEAD()�� �Լ�, �̸�, get_owner()�� �����Ѵ�. 
		// get_owner()�� �����ϰ� ������ Ȯ���Ѵ�. 
		// 
	}
}