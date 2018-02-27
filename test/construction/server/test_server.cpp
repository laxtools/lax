#include "stdafx.h"
#include <lax/server/server.hpp>
#include <lax/util/logger.hpp>
#include <lax/net/service.hpp>
#include <catch.hpp>
#include <fstream>

TEST_CASE("test server")
{
	SECTION("test server load")
	{
		auto cfg = R"(
{
  "server_1": {
    "id": 77,
    "listens": [
      {
        "protocol": "bits",
        "address": "0.0.0.0:7777"
      }
    ],
    "scheduler": {
      "runner_count": 1,
      "idle_check_threshold_time": 0.001,
      "single_loop_run_limit": 0,
      "start_task_when_schedule_called": false
    },
    "peer_service": {
		"name" : "peer_service", 
		"peers" : 
		[
			{
				"protocol": "bits",
				"address": "127.0.0.1:7778", 
				"reconnect_interval": 10
			}
		]
    }, 
    "services": {
      "dummy_1" : {
        "type": "dummy",
		"config" : {
	    }
      }, 
      "dummy_2" : { 
        "type": "dummy",
		"config" : {
	    }
      }
	}
  },
  "local": { "name": "server_1" }
})";

		try
		{

			auto jcfg = nlm::json::parse(cfg);
			auto jlocal = jcfg["local"];
			auto jname = jlocal["name"];
			auto jserver = jcfg[jname.get<std::string>()];

			CHECK(jserver["id"] == 77);

			lax::server::server server(jname.get<std::string>(), jserver);

			CHECK(server.get_id() == 77);

			bool rc = server.start();

			REQUIRE(rc);
			REQUIRE(lax::net::service::inst().get_acceptor_count() == 1);

			server.execute();

			server.finish();
		}
		catch (nlm::json::exception& ex)
		{
			lax::util::log()->critical("exception: {}", ex.what());
		}
		catch (lax::util::exception& ex)
		{
			lax::util::log()->critical("exception: {}", ex.what());
		}
	}

	SECTION("configuration json from file")
	{
		try
		{
			// default execution folder: build/construction 

			std::ifstream ifs("json1.json");

			auto jcfg = nlm::json::parse(ifs);
			auto jlocal = jcfg["local"];
			auto jname = jlocal["name"];
			auto jserver = jcfg[jname.get<std::string>()];

			CHECK(jserver["id"] == 77);
		}
		catch (nlm::json::exception& ex)
		{
			lax::util::log()->critical("exception: {}", ex.what());
		}
	}

	SECTION("test peer connections")
	{
		//
		// 2 servers w/ configurations
		// - test1.json 
		// - test2.json
		// 


		// 
		// timer callback required
		// channel is the best place
		// callback with a message 
		//

		// start option: use existing net::service  
		// 
	}

	SECTION("test loading services")
	{

	}

	SECTION("test service usage")
	{
		// announce and service directory

		// remote and local reference

		// close / state change 

	}
}