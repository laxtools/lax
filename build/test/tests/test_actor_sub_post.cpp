#include "stdafx.h"

#include <lax/actor/actor.h>
#include <lax/util/simple_timer.h>
#include <lax/util/logger.h>
#include <catch.hpp>
#include <iostream>
#include <functional>

using namespace lax::actor;
using namespace lax::net;

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

TEST_CASE("test posting")
{
	SECTION("msg posting through actor")
	{

	}


}

TEST_CASE("test mailbox")
{
	// TODO: 편의 함수들 추가

	SECTION("enhance - type / seq ")
	{
		// 키 만으로 하면 등록이 많아진다. 
		// type 기반으로 같이 처리할 수 있도록 한다.
		// key가 type / seq로 이루어졌다고 보고 
		// 각각에 대해 처리 가능하게 한다. 
		// 16 bit / 16 bit로 tag 단계에서 분리 처리

		// type 값만 지정할 수 있도록 해서 tag 지정 가능

		// 실수하지 않고 쉽게 사용할 수 있도록 정리 
		// - 

	}

	SECTION("enhance - id based subscrption")
	{
		// msg_addr_base로 캐스팅해서 처리

		// 타잎 별로 처리가능

		// 
	}

	SECTION("usage")
	{
		post::target targ;
		post::tag tv;

		tv.set(protocol::json, "hello");

		std::string msg;

		post::target::exec exec = [&msg](msg::ptr m) 
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			msg = m2->get_msg()[0].string_value();
		};

		targ.set_exec(exec);

		dummy dum; 

		auto id = dum.get_mailbox_for_test_only().subscribe(tv, targ);

		REQUIRE(id > 0);

		std::string m = R"({"key":"hello", "msg":["world"]})";
		m.append("\r\n");

		auto mp = std::make_shared<msg_json>();
		REQUIRE(mp->unpack(m));

		// 메일 박스로 직접 전송
		REQUIRE(dum.get_mailbox_for_test_only().post(mp) == 1); // post count 

		REQUIRE(msg == "world");
	}

	SECTION("multiple subscription on same key")
	{
		post::target targ;
		post::tag tv;

		tv.set(protocol::json, "hello");

		std::string msg1;
		std::string msg2;

		post::target::exec exec1 = [&msg1](msg::ptr m) 
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			msg1 = m2->get_msg()[0].string_value();
		};
		
		post::target::exec exec2 = [&msg2](msg::ptr m) 
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			msg2 = m2->get_msg()[0].string_value();
		};

		targ.set_exec(exec1);

		dummy dum; 

		auto id1 = dum.get_mailbox_for_test_only().subscribe(tv, targ);
		REQUIRE(id1 > 0);

		targ.set_exec(exec2);
		auto id2 = dum.get_mailbox_for_test_only().subscribe(tv, targ);
		REQUIRE(id2 > 0);

		std::string m = R"({"key":"hello", "msg":["world"]})";
		m.append("\r\n");

		auto mp = std::make_shared<msg_json>();
		REQUIRE(mp->unpack(m));

		// 메일 박스로 직접 전송
		REQUIRE(dum.get_mailbox_for_test_only().post(mp) == 2); // post count 

		REQUIRE(msg1 == "world");
		REQUIRE(msg2 == "world");
	}

	SECTION("dispatch w/ condition")
	{
		post::target targ;
		post::tag tv;

		tv.set(protocol::json, "hello");

		std::string msg;

		post::target::cond cnd = [](msg::ptr m, post::target::value v)
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			return m2->get_msg()[0].int_value() == v;
		};

		post::target::exec exec = [&msg](msg::ptr m)
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			msg = m2->get_msg()[0].string_value();
		};

		targ.set_cond(cnd, 3);
		targ.set_exec(exec);

		dummy dum;

		auto id = dum.get_mailbox_for_test_only().subscribe(tv, targ);

		REQUIRE(id > 0);

		// match case
		{
			std::string m = R"({"key":"hello", "msg":[3, "world"]})";
			m.append("\r\n");

			auto mp = std::make_shared<msg_json>();
			REQUIRE(mp->unpack(m));

			// 메일 박스로 직접 전송
			REQUIRE(dum.get_mailbox_for_test_only().post(mp) == 1); // post count 
		}

		// unmatch case
		{
			std::string m = R"({"key":"hello", "msg":[5, "world"]})";
			m.append("\r\n");

			auto mp = std::make_shared<msg_json>();
			REQUIRE(mp->unpack(m));

			// 메일 박스로 직접 전송
			REQUIRE(dum.get_mailbox_for_test_only().post(mp) == 0); // post count 
		}
	}

	SECTION("simple performance")
	{

		post::target targ;
		post::tag tv;

		tv.set(protocol::json, "hello");

		std::string msg;

		post::target::exec exec = [&msg](msg::ptr m)
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			msg = m2->get_msg()[0].string_value();
		};

		targ.set_exec(exec);

		dummy dum;

		auto id = dum.get_mailbox_for_test_only().subscribe(tv, targ);

		std::string m = R"({"key":"hello", "msg":["world"]})";
		m.append("\r\n");

		auto mp = std::make_shared<msg_json>();
		mp->unpack(m);

		// 메일 박스로 직접 전송

		lax::util::simple_timer timer;

		const auto test_count = 100;

		for (int i = 0; i < test_count; ++i)
		{
			dum.get_mailbox_for_test_only().post(mp);
		}

		lax::util::log()->info("simple dispatching elapsed: {0}", timer.elapsed());

		// Deubg. 3.7초
		// Release. 0.03초

		// 디버그와 릴리스 차이가 상당히 크지만 성능은 만족할 만한 수준
		// 한번 호출. 3 nano 초
	}

	SECTION("thread performance")
	{
		post::target targ;
		post::tag tv;

		tv.set(protocol::json, "hello");

		std::string msg;

		post::target::exec exec = [&msg](msg::ptr m)
		{
			auto m2 = std::static_pointer_cast<msg_json>(m);

			msg = m2->get_msg()[0].string_value();
		};

		targ.set_exec(exec);

		dummy dum;

		auto id = dum.get_mailbox_for_test_only().subscribe(tv, targ);

		std::string m = R"({"key":"hello", "msg":["world"]})";
		m.append("\r\n");

		auto mp = std::make_shared<msg_json>();
		mp->unpack(m);

		// 메일 박스로 직접 전송

		lax::util::simple_timer timer;

		const auto test_count = 100;

		std::function<void()>	func = [test_count, &dum, &mp]() {
			for (int i = 0; i < test_count; ++i)
			{
				dum.get_mailbox_for_test_only().post(mp);
			}
		};

		const auto thread_count = 8;

		std::thread threads[thread_count];

		for (int i = 0; i < thread_count; ++i)
		{
			threads[i].swap(std::thread(func));
		}

		for (int i = 0; i < thread_count; ++i)
		{
			threads[i].join();
		}

		lax::util::log()->info(
			"thread dispatching elapsed: {0} threads: {1}", 
			timer.elapsed(), thread_count
		);

		// 8개 쓰레드. Release. 0.283 --> 쓰레드별 0.035초 . 0.005초 정도 증가
		// 
		// 약간 경합이 생기긴 하지만 shared_timed_mutex 괜찮음
		// 
	}
}

TEST_CASE("actor posting design")
{
	SECTION("design")
	{
		// 
		// per protocol 
		// 
		// per msg
		//  - key 
		//  - content

		// type recovery

		SECTION("tag / target / field lookup")
		{
			// tag
			// - protocol(8) ikey(32) skey(std::string)

			// tag dispatching
			// - <tag, target>

			// target
			// - function 
			// - optional field lookup function
			//	 - <value, func>

			// field lookup function
			// - (tag, msg) -> value
		}

		SECTION("usage")
		{
			SECTION("target / sub / post")
			{
				// target tgt;

				// tgt.add_cond(func, value )
				// tgt.add_exec(func ); 

				// subscripition id = actor::sub( tag, target )
			}

			SECTION("unsubscribe")
			{
				// actor::unsub(subscription id)
				
				// 자동으로 관리하기 위한 처리 필요
				// id에 대한 의미를 갖고 있어야 해제할 수 있다. 
				// 적어도 소멸할 때는 자동해제 되어야 한다. 
			}
		}

		SECTION("debugging")
		{
			// setup log 
			// 
			// call log 
			//	- condition function
			//  - exec function 

			// stats
			// 
		}

		SECTION("thread")
		{
			// 1안. sub / unsub passes through a queue (local message)
			// 2안. use shared lock  
			//  - move sub/unsub to mailbox
			//  - post w/ mailbox

			// 2안이 깔끔하다. 왜냐하면 shared lock만 주로 사용하는 패턴

			//
			// 단위 테스트가 보다 수월해졌다. 위임이 항상 거의 답이다. 
			// mailbox로 넘기고 포함하도록 하고 필요하면 얻어 쓰도록 했다. 
			// 이런 방식의 분리, 위임 방식은 항상 좋다. 
			// 
		}

		SECTION("look at Rx and fluent")
		{
			// subscription must be easier and more stable than as it is now

		}
	}
}

