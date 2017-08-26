#include "stdafx.h"

#include <lax/net/service.h>
#include <lax/net/protocol/json/session_json.h>
#include <lax/util/logger.h>
#include "test_app.h"

#include <catch.hpp>
#include <conio.h>
#include <memory>
#include <iostream>
#include <map>
#include <thread>

using namespace lax::net;

namespace
{


struct test_app : public lax::test::test_app
{
	test_app(bool send)
		: send_(send)
	{
	}

	/// called when session is ready 
	virtual void on_ready(const session::id& id) override 
	{
		send_json(id);
	}

	/// called when session has a message to handle
	virtual void on_recv(const session::id& id, msg::ptr m) override
	{
		send_json(id);
	}

	void send_json(const session::id& id)
	{
		return_if(!send_);

		std::string m = R"({"key":"path/to/target", "msg":["a", 123, true, false, null]})";
		m.append("\r\n");

		auto mp = std::make_shared<msg_json>();
		auto rc = mp->unpack(m);

		check(rc);
		check(svc_);

		auto session1 = std::static_pointer_cast<session_json>(svc_->get(id));
		rc = session1->send(mp);

		check(rc);
	}

	void set(service* svc)
	{
		svc_ = svc;
	}

	service* svc_;
	bool send_ = false;
};


} // noname

TEST_CASE("json session protocol pack")
{
	test_app			ap(false);
	service				svc(&ap);
	asio::io_service	ios;
	tcp::socket			soc(ios);

	// 가짜로 생성해서 메세지 파싱 부분만 테스트 
	session_json sj(svc, session::id(10), std::move(soc), false); 

	SECTION("simple recv")
	{
		std::string m = R"({"key":"path/to/target", "msg":["a", 123, true, false, null]})";
		m.append("\r\n");

		sj.test_recv((uint8_t*)m.c_str(), m.length());

		REQUIRE(sj.get_buf_size() == 0);

		auto rm = sj.get_last_msg();
		auto lm = std::static_pointer_cast<msg_json>(rm);

		REQUIRE(lm->get_key() == "path/to/target");

		auto msg_part = lm->get_msg();
		auto first_value = msg_part[0];

		REQUIRE(first_value.string_value() == "a");
	}

	SECTION("divided recv")
	{
		std::string m = R"({"key":"path/to/target", "msg":["a", 123, true, false, null]})";
		m.append("\r\n");

		sj.test_recv((uint8_t*)m.c_str(), 16);
		REQUIRE(sj.get_buf_size() == 16);

		sj.test_recv((uint8_t*)(m.c_str() + 16), m.length() - 16);
		REQUIRE(sj.get_buf_size() == 0);

		auto rm = sj.get_last_msg();
		auto lm = std::static_pointer_cast<msg_json>(rm);

		REQUIRE(lm->get_key() == "path/to/target");

		auto msg_part = lm->get_msg();
		auto first_value = msg_part[0];

		REQUIRE(first_value.string_value() == "a");
	}

	SECTION("multiple recv same time")
	{
		std::string m = R"({"key":"path/to/target", "msg":["a", 123, true, false, null]})";
		m.append("\r\n");

		for (int i = 0; i < 8; ++i)
		{
			sj.test_recv((uint8_t*)m.c_str(), m.length());
		}

		auto rm = sj.get_last_msg();
		auto lm = std::static_pointer_cast<msg_json>(rm);

		REQUIRE(lm->get_key() == "path/to/target");

		auto msg_part = lm->get_msg();
		auto first_value = msg_part[0];

		REQUIRE(first_value.string_value() == "a");

		REQUIRE(sj.get_recv_count() == 8);
	}

	SECTION("invalid recv")
	{
		SECTION("invalid json")
		{
			std::string m = R"("path/to/target", "msg":["a", 123, true, false, null]})";
			m.append("\r\n");

			sj.test_recv((uint8_t*)m.c_str(), m.length());

			REQUIRE(sj.get_buf_size() == 0);

			REQUIRE(sj.get_recv_count() == 0);
		}

		SECTION("invalid msg")
		{
			std::string m = R"({"m":["a", 123, true, false, null]})";
			m.append("\r\n");

			sj.test_recv((uint8_t*)m.c_str(), m.length());

			REQUIRE(sj.get_recv_count() == 0);
		}
	}

	SECTION("overflow")
	{
		for (int i = 0; i < 1024; ++i)
		{
			sj.test_recv((uint8_t*)"01234567890", 10);
		}

		REQUIRE(!sj.is_open());
	}

	SECTION("message pack and unpack")
	{
		const json::Json obj = json::Json::object({
			{ "key", "v1" },
			{ "msg", json::Json::array({ "a", 123.0, true, false, nullptr }) }
		});

		auto m = std::make_shared<msg_json>();

		REQUIRE(m->pack(obj));

		std::string s = m->get_str(); // copy

		REQUIRE(m->unpack(s));
	}

	SECTION("message pack string recv simulation")
	{
		const json::Json obj = json::Json::object({
			{ "key", "path/to/target" },
			{ "msg", json::Json::array({ "a", 123.0, true, false, nullptr }) }
		});

		auto m = std::make_shared<msg_json>();

		m->pack(obj);

		std::string s = m->get_str(); // copy

		sj.test_recv((uint8_t*)s.c_str(), s.length());

		auto rm = sj.get_last_msg();
		auto lm = std::static_pointer_cast<msg_json>(rm);

		REQUIRE(lm->get_key() == "path/to/target");

		auto msg_part = lm->get_msg();
		auto first_value = msg_part[0];

		REQUIRE(first_value.string_value() == "a");
	}
}

TEST_CASE("json live send / recv")
{
	test_app app(true);

	service svc(&app);

	app.set(&svc); // use service to send

	service::creator func = [] // vs 2015 intellisense 오류
	(
		service& svc,
		const session::id& id,
		tcp::socket&& soc,
		bool accepted
		)
	{
		return session::ptr(
			new session_json(
				svc,
				id,
				std::move(soc),
				accepted));
	};

	REQUIRE(svc.listen("127.0.0.1:7777", func));
	REQUIRE(svc.connect("127.0.0.1:7777", func));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	auto session1 = std::static_pointer_cast<session_json>(svc.get(session::id(1, 0)));

	const int test_count = 1000;

	while (session1->get_recv_count() < test_count)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// 생각보다 mp가 깨지는 경우가 많다. session1에는 제대로 들어있다. 
		// 
		auto mp = std::static_pointer_cast<msg_json>(session1->get_last_msg());

		if (mp)
		{
			REQUIRE(mp->get_key() == "path/to/target");
		}
	}
}
