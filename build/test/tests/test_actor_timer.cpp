
#include "stdafx.h"

#include <lax/actor/actor.h>
#include <catch.hpp>

using namespace lax::actor;

namespace
{

class dummy : public actor
{
public: 
	dummy()
		: actor(33)
	{
	}
	

private: 
	void on_process() override
	{
	}
};

}

TEST_CASE("actor timer")
{
	SECTION("design")
	{
		// features: 
		// - per actor timer
		// - check timers fast

		// impl: 
		// - slots 
		//  - ���� ���� ���� ����. 100ms, 200ms, 300ms, 400ms, 500ms, ... 2200ms
		//  - �켱 ���� ť�� ���� ���� �ð� �ּ� �� ������ ���� 
		//    - 100ms, 120ms, 500ms, 800ms .. 
		//    - ť ���� ���� �׸��� ���� �ð��� �� �Ǿ��ٸ� ��ü�� ���� �ð��� �� ��
		//  
	}

	SECTION("simple example")
	{
		dummy dum;

		int v = 0;

		auto id = dum.get_timer_for_test_only().set(0.1f);

		timer::action act = [&v](int idv) { idv; v++;};

		dum.get_timer_for_test_only().add(id, act);

		REQUIRE(dum.get_timer_for_test_only().get_last_run_tick(id) == 0.0f);
		REQUIRE(dum.get_timer_for_test_only().get_next_run_tick(id) == 0.1f);

		REQUIRE(dum.get_timer_for_test_only().get_slot_count() == 1);
		REQUIRE(dum.get_timer_for_test_only().get_req_count() == 1);

		dum.process(0.2f);
		dum.process(0.4f);
		dum.process(0.6f);

		REQUIRE(dum.get_timer_for_test_only().get_last_run_tick(id) == 0.6f);
		REQUIRE(dum.get_timer_for_test_only().get_run_count(id) == 3);
		REQUIRE(v == 3);
	}

	SECTION("duration")
	{
		dummy dum;

		int v = 0;

		auto id = dum.get_timer_for_test_only().set(0.1f, 0.1f, false);

		REQUIRE(dum.get_timer_for_test_only().get_last_run_tick(id) == 0.0f);
		REQUIRE(dum.get_timer_for_test_only().get_next_run_tick(id) == 0.1f);

		REQUIRE(dum.get_timer_for_test_only().get_slot_count() == 1);
		REQUIRE(dum.get_timer_for_test_only().get_req_count() == 1);

		dum.process(0.3f);

		REQUIRE(dum.get_timer_for_test_only().has(id) == false);
	}

	SECTION("cancel")
	{
		dummy dum;

		int v = 0;

		auto id = dum.get_timer_for_test_only().set(0.1f, 1.f, 1, false);

		timer::action act = [&v](int idv) { idv; v++;};

		dum.get_timer_for_test_only().add(id, act);

		REQUIRE(dum.get_timer_for_test_only().cancel(id));

		dum.process(0.3f);

		REQUIRE(dum.get_timer_for_test_only().get_run_count(id) == 0);
	}

	
}

