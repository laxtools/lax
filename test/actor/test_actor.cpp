#include "stdafx.h"
#include <catch.hpp>
#include <lax/actor/actor.hpp>
#include <lax/actor/task_adapter.hpp>
#include <iostream>

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

class skill_comp : public component
{
public:
	COMPONENT_HEAD(skill_comp); // name, ptr, get_owner

	virtual void cast()
	{
		std::cout << "skill_comp::cast" << std::endl;
	}
};

class pc_skill_comp : public skill_comp
{
public:
	COMPONENT_HEAD_INH(pc_skill_comp, skill_comp); // name, ptr, get_owner

	void cast() override
	{
		std::cout << "pc_skill_comp::cast" << std::endl;
	}
};


} // noname

TEST_CASE("test actor")
{
	// 실제 게임을 만들면서 필요한 기능을 확장한다.
	// 몇 가지 원칙만 정리한다.
	// - type safety를 최대한 유지하면서 다형성을 지원한다.
	// - 컴포넌트가 필요 없으면 actor 레벨 처리로 완결한다.
	//   - 재사용되지 않는 컴포넌트는 기능 분할 의미만 있다.
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
		// actor를 task_scheduler에 등록할 때 사용
		//
	}

	SECTION("component")
	{
		auto ap = std::make_shared<test_actor>();

		ap->add_comp<movement_comp>();

		auto movement = ap->get_comp<movement_comp>();
		REQUIRE(!!movement);

		if (movement)
		{
			movement->move();
		}

		auto skill = ap->get_comp<skill_comp>();
		REQUIRE(!skill);

		//
		// 컴포넌트의 사용: 
		// c++에서 잘못 컨벤션이 잡히면 컴포넌트는 사용하지 않는 게 오히려 낫다.
		// 그냥 멤버 함수로 하위 클래스를 작성하는 것이 더 나을 수 있다. 
		// 따라서, 다음과 같이 정리하고 이후 개선해 나간다. 
		// - actor의 생성자에서만 특별한 경우가 아니라면 add_comp<Comp>()로 
		//   모두 등록한다. 
		// - 사용할 때는 get_comp<Comp>()로 하고 포인터 체크 후 사용한다. 
		// - actor들에 적용할 경우는 actor::for_each<Comp>를 사용한다.
		//
		// 컴포넌트의 정의:
		// COMPONENT_HEAD()로 함수, 이름, get_owner()를 정의한다. 
		// get_owner()로 안전하게 참조를 확보한다. 
		// 
	}

	SECTION("component inheritance")
	{
		auto ap = std::make_shared<test_actor>();

		ap->add_comp<pc_skill_comp>();

		auto base_skill = ap->get_comp<skill_comp>();
		REQUIRE(base_skill);

		base_skill->cast();
	}
}