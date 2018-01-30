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

	// �⺻ ���� ������ shared_ptr �ڵ� ������ �� �� (�⺻ �����ڸ� �ʿ�� ��)
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
		// actor�� task_scheduler�� ����� �� ���
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

		// component�� ���� ��: 
		// - using ptr = std::shared_ptr<cls>; �ʿ� 
		// - �����ڵ鿡�� push_type<cls>(); ����� �� 
		// 

		//
		// �� �� ������ ��� ���� ���� Ÿ�ٿ� ���� 
		// ������Ʈ�� ���� �����ϰ� ��� ����
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
		// �������̽� ������Ʈ�� ���� ó�� ��������
		//   

		// 
		// apply_components<skill_comp>()�� ����ϸ� ���� �����ϰ� ó�� ���� 
		// - �̹� �ִ� Ÿ�ٵ鿡 ���ؼ� ����. 
		//
	}
}