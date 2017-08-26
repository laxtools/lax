#include "stdafx.h"

#include <funz/net/service.h>
#include <funz/net/server.h>
#include <funz/net/client.h>
#include <funz/net/protocol/json/session_json.h>
#include <funz/util/logger.h>

#include <catch.hpp>
#include <conio.h>
#include <memory>
#include <iostream>
#include <map>
#include <thread>

using namespace funz::net;

namespace
{


struct test_app : public funz::net::app
{
	/// called when session is ready 
	virtual void on_ready(const session::id& id) override {}

	/// called when session is closed  
	virtual void on_closed(const session::id& id, asio::error_code ec) override {}

	/// called when session has a message to handle
	virtual void on_recv(const session::id& id, msg::ptr m) override {}

	/// called when accept failed
	virtual void on_accept_failed(const std::string& addr) override {}

	/// called when connect failed  
	virtual void on_connect_failed(const std::string& addr) override {}
};


} // noname

TEST_CASE("json message")
{
	test_app ap;

	service svc(&ap);

	service::creator func = [] // vs 2015 intellisense ¿À·ù
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

	// slow text protocol 

	// parse into json text
	// pass them around

	// get two sessions 
	// send / recv 

	// sub / post w/ session. 
	//  
}