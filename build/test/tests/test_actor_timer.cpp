
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
		//  - 실행 간격 별로 실행. 100ms, 200ms, 300ms, 400ms, 500ms, ... 2200ms
		//  - 우선 순위 큐에 다음 실행 시간 최소 값 순으로 지정 
		//    - 100ms, 120ms, 500ms, 800ms .. 
		//    - 큐 제일 앞의 항목이 아직 시간이 안 되었다면 전체가 아직 시간이 안 됨
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

