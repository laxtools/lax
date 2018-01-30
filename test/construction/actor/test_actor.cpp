#include "stdafx.h"
#include <catch.hpp>
#include <lax/actor/actor.hpp>
#include <lax/actor/task_adaptor.hpp>
#include <iostream>

using namespace lax::actor;

namespace
{

class test_actor : public actor
{
public:
	using ptr = std::shared_ptr<test_actor>;

	// 기본 값이 없으면 shared_ptr 코드 생성이 안 됨 (기본 생성자를 필요로 함)
	test_actor(weak_ptr parent = weak_ptr())
		: actor(parent)
	{
		push_type<test_actor>();
	}

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
	using ptr = std::shared_ptr<movement_comp>;

	movement_comp(actor& owner)
		: component(owner)
	{
		push_type<movement_comp>();
	}

	void move()
	{
	}
};

class skill_comp : public component
{
public:
	using ptr = std::shared_ptr<skill_comp>;

	skill_comp(actor& owner)
	: component(owner)
	{
		push_type<skill_comp>();
	}

	virtual void cast()
	{
		std::cout << "skill_comp::cast" << std::endl;
	}
};

class pc_skill_comp : public skill_comp
{
public:
	using ptr = std::shared_ptr<pc_skill_comp>;

	pc_skill_comp(actor& owner)
		: skill_comp(owner)
	{
		push_type<pc_skill_comp>();
	}

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

		ap->execute();

		ap->finish();
	}

	SECTION("task actor")
	{
		auto ap = std::make_shared<test_actor>();
		auto tp = std::make_shared<task_adaptor>(ap);

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

		ap->add_component<movement_comp>();

		auto movement = ap->get_component<movement_comp>();
		REQUIRE(!!movement);

		if (movement)
		{
			movement->move();
		}

		auto skill = ap->get_component<skill_comp>();
		REQUIRE(!skill);

		// component를 만들 때: 
		// - using ptr = std::shared_ptr<cls>; 필요 
		// - 생성자들에서 push_type<cls>(); 해줘야 함 
		// 

		//
		// 위 두 가지로 상속 계층 상의 타잎에 대해 
		// 컴포넌트를 비교적 안전하게 사용 가능
		//
	}

	SECTION("component inheritance")
	{
		SECTION("single comp")
		{
			auto ap = std::make_shared<test_actor>();

			ap->add_component<pc_skill_comp>();

			auto base_skill = ap->get_component<skill_comp>();
			REQUIRE(base_skill);

			base_skill->cast();
		}

		SECTION("several same type comp")
		{
			auto ap = std::make_shared<test_actor>();

			ap->add_component<pc_skill_comp>();

			ap->add_component<pc_skill_comp>();

			auto vec = ap->get_components<skill_comp>();
			REQUIRE(vec.size() == 2);

			vec[0]->cast();
		}

		//
		// 인터페이스 컴포넌트에 대해 처리 가능해짐
		//   

		// 
		// apply_components<skill_comp>()를 사용하면 보다 안전하게 처리 가능 
		// - 이미 있는 타잎들에 대해서 동작. 
		//
	}
}