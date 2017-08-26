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
	// TODO: ���� �Լ��� �߰�

	SECTION("enhance - type / seq ")
	{
		// Ű ������ �ϸ� ����� ��������. 
		// type ������� ���� ó���� �� �ֵ��� �Ѵ�.
		// key�� type / seq�� �̷�����ٰ� ���� 
		// ������ ���� ó�� �����ϰ� �Ѵ�. 
		// 16 bit / 16 bit�� tag �ܰ迡�� �и� ó��

		// type ���� ������ �� �ֵ��� �ؼ� tag ���� ����

		// �Ǽ����� �ʰ� ���� ����� �� �ֵ��� ���� 
		// - 

	}

	SECTION("enhance - id based subscrption")
	{
		// msg_addr_base�� ĳ�����ؼ� ó��

		// Ÿ�� ���� ó������

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

		// ���� �ڽ��� ���� ����
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

		// ���� �ڽ��� ���� ����
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

			// ���� �ڽ��� ���� ����
			REQUIRE(dum.get_mailbox_for_test_only().post(mp) == 1); // post count 
		}

		// unmatch case
		{
			std::string m = R"({"key":"hello", "msg":[5, "world"]})";
			m.append("\r\n");

			auto mp = std::make_shared<msg_json>();
			REQUIRE(mp->unpack(m));

			// ���� �ڽ��� ���� ����
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

		// ���� �ڽ��� ���� ����

		lax::util::simple_timer timer;

		const auto test_count = 100;

		for (int i = 0; i < test_count; ++i)
		{
			dum.get_mailbox_for_test_only().post(mp);
		}

		lax::util::log()->info("simple dispatching elapsed: {0}", timer.elapsed());

		// Deubg. 3.7��
		// Release. 0.03��

		// ����׿� ������ ���̰� ����� ũ���� ������ ������ ���� ����
		// �ѹ� ȣ��. 3 nano ��
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

		// ���� �ڽ��� ���� ����

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

		// 8�� ������. Release. 0.283 --> �����庰 0.035�� . 0.005�� ���� ����
		// 
		// �ణ ������ ����� ������ shared_timed_mutex ������
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
				
				// �ڵ����� �����ϱ� ���� ó�� �ʿ�
				// id�� ���� �ǹ̸� ���� �־�� ������ �� �ִ�. 
				// ��� �Ҹ��� ���� �ڵ����� �Ǿ�� �Ѵ�. 
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
			// 1��. sub / unsub passes through a queue (local message)
			// 2��. use shared lock  
			//  - move sub/unsub to mailbox
			//  - post w/ mailbox

			// 2���� ����ϴ�. �ֳ��ϸ� shared lock�� �ַ� ����ϴ� ����

			//
			// ���� �׽�Ʈ�� ���� ����������. ������ �׻� ���� ���̴�. 
			// mailbox�� �ѱ�� �����ϵ��� �ϰ� �ʿ��ϸ� ��� ������ �ߴ�. 
			// �̷� ����� �и�, ���� ����� �׻� ����. 
			// 
		}

		SECTION("look at Rx and fluent")
		{
			// subscription must be easier and more stable than as it is now

		}
	}
}

