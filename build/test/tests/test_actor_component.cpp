#include "stdafx.h"

#include <lax/actor/actor.h>
#include <catch.hpp>

using namespace lax::actor;

namespace
{

struct test_comp : public component
{
	using ptr = std::shared_ptr<test_comp>;

	COMPONENT_BODY_OVERRIDE(33);

	void test()
	{
		++count;
	}

	int count = 0;
};

struct actor_a : public actor
{
	actor_a()
		: actor(0)
	{
		init<test_comp>();	// new and add
	}

	void on_process()
	{
	}

	test_comp::ptr test;
};

struct actor_b : public actor 
{
	actor_b()
		: actor(0)
	{
		// no component
	}

	void on_process()
	{
	}

	void engage(actor::ptr enemy)
	{
		return_if(!enemy->has_comp(33));

		enemy->get_comp<test_comp>()->test();
	}
};

}

TEST_CASE("more or less safe component")
{
	auto a = std::make_shared<actor_a>();
	auto b = std::make_shared<actor_b>();

	a->process(1.0f);
	b->process(1.0f);

	b->engage(a);

	REQUIRE(a->get_comp<test_comp>()->count == 1);
	REQUIRE(!b->has_comp(test_comp::key_type));
}
