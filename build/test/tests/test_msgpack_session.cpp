#include "stdafx.h"

#include <lax/net/service.h>
#include <lax/net/protocol/msgpack/session_pack.h>
#include <lax/net/protocol/msgpack/msg_pack_factory.h>
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

struct msg_echo : public msg_pack
{
	int iv;
	float fv;
	std::string cv;

	MSGPACK_KEY(1, 33);
	MSGPACK_BODY_OVERRIDE(msg_echo);
	MSGPACK_DEFINE(iv, fv, cv);
};

struct test_app : public lax::test::test_app
{
	test_app(bool send)
		: send_(send)
	{
	}

	/// called when session is ready 
	virtual void on_ready(const session::id& id) override
	{
		send_echo(id);
	}

	/// called when session has a message to handle
	virtual void on_recv(const session::id& id, msg::ptr m) override
	{
		send_echo(id);
	}

	void send_echo(const session::id& id)
	{
		return_if(!send_);

		auto mp = std::make_shared<msg_echo>();
		mp->iv = 9;

		check(svc_);

		auto session1 = std::static_pointer_cast<session_pack>(svc_->get(id));
		auto rc = session1->send(mp);

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

TEST_CASE("msgpack session protocol pack")
{
	test_app			ap(false);
	service				svc(&ap);
	asio::io_service	ios;
	tcp::socket			soc(ios);

	// 가짜로 생성해서 메세지 파싱 부분만 테스트 
	session_pack sp(svc, session::id(10), std::move(soc), false);

	MSGPACK_REGISTER(msg_echo);

	SECTION("simple recv")
	{
		msg_echo echo;
		echo.iv = 33;
		echo.cv = "Hello msgpack!";

		msg_pack::buffer buf;

		sp.test_recv(echo, buf);
		sp.test_recv(buf.data(), buf.size());
		
		auto m = sp.get_last_msg<msg_echo>();
		REQUIRE(m);
		REQUIRE(m->iv == 33);
		REQUIRE(m->cv == "Hello msgpack!");

		// REQUIRE
	}

	SECTION("divided recv")
	{
		msg_echo echo;
		echo.iv = 33;
		echo.cv = "Hello msgpack!";

		msg_pack::buffer buf;

		sp.test_recv(echo, buf);

		const char* dp = buf.data();
		sp.test_recv(dp, 20);
		sp.test_recv(dp+20, buf.size()-20);

		auto m = sp.get_last_msg<msg_echo>();
		REQUIRE(m);
		REQUIRE(m->iv == 33);
		REQUIRE(m->cv == "Hello msgpack!");
	}

	SECTION("multiple recv same time")
	{
		msg_echo echo;
		echo.cv = "Hello msgpack!";

		msg_pack::buffer buf[3];

		echo.iv = 33;
		sp.test_recv(echo, buf[0]);

		echo.iv = 34;
		sp.test_recv(echo, buf[1]);

		echo.iv = 35;
		sp.test_recv(echo, buf[2]);

		msg_pack::buffer buf_l;

		for ( int i=0; i<3; ++i)
		{ 
			buf_l.write(buf[i].data(), buf[i].size());
		}

		sp.test_recv(buf_l.data(), buf_l.size());
		auto m = sp.get_last_msg<msg_echo>();
		REQUIRE(m);
		REQUIRE(m->iv == 35);
		REQUIRE(m->cv == "Hello msgpack!");
	}

	SECTION("invalid recv")
	{
		SECTION("invalid format")
		{
			// broken (무시됨)
			{
				msg_echo echo;
				echo.iv = 33;
				echo.cv = "Hello msgpack!";

				msg_pack::buffer buf;

				sp.test_recv(echo, buf);

				char* p = buf.data();
				p[8] = '0'; // break it

				sp.test_recv(buf.data(), buf.size());

				auto m = sp.get_last_msg<msg_echo>();
				REQUIRE(!m);
			}

			// recover
			{
				msg_echo echo;
				echo.iv = 90;
				echo.cv = "Hello msgpack!";

				msg_pack::buffer buf;

				sp.test_recv(echo, buf);

				sp.test_recv(buf.data(), buf.size());

				auto m = sp.get_last_msg<msg_echo>();
				REQUIRE(m);
				REQUIRE(m->iv == 90);
			}
		}
	}
}

TEST_CASE("msgpack live send / recv")
{
	test_app app(true);

	service svc(&app);

	app.set(&svc); // use service to send

	MSGPACK_REGISTER(msg_echo);

	service::creator func = [] // vs 2015 intellisense 오류
	(
		service& svc,
		const session::id& id,
		tcp::socket&& soc,
		bool accepted
		)
	{
		return session::ptr(
			new session_pack(
				svc,
				id,
				std::move(soc),
				accepted));
	};


	REQUIRE(svc.listen("127.0.0.1:7777", func));
	REQUIRE(svc.connect("127.0.0.1:7777", func));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	auto session1 = std::static_pointer_cast<session_pack>(svc.get(session::id(1, 0)));

	const int test_count = 1000;

	while (session1->get_recv_count() < test_count)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		auto mp = std::static_pointer_cast<msg_echo>(session1->get_last_msg());

		if (mp)
		{
			REQUIRE(mp->iv == 9);
		}
	}
}
