#include "stdafx.h"
#include <lax/server/server.hpp>
#include <catch.hpp>

TEST_CASE("test server")
{
	SECTION("test server load")
	{
		auto cfg = R"(
{
  "server_1": {
    "id": 1,
    "listens": [
      {
        "protocol": "bits",
        "port": 7777
      }
    ],
    "scheduler": {
      "runner_count": 2,
      "idle_check_threshold_time": 0.001,
      "single_loop_run_limit": 0,
      "start_task_when_schedule_called": true
    },
    "peers": [
      {
        "protocol": "bits",
        "address": "127.0.0.1:7778"
      }
    ]
  },
  "local": { "name": "server_1" }
})";

		auto jcfg = nlm::json::parse(cfg);
		auto jlocal = jcfg["local"];
		auto jname = jlocal["name"];
		auto jserver = jcfg[jname.get<std::string>()];

		CHECK(jserver["id"] == 1);

		lax::server::server server("name", jserver);

		CHECK(server.get_id() == 1);

		server.start();
		server.execute();
		server.finish();
	}

	SECTION("test peer connections")
	{
		//
		// timer callback required
		// channel is the best place
		// callback with a message 
		// efficient timer management per channel
		//
	}

	SECTION("test service announcement")
	{
	}

	SECTION("source monitor")
	{
		// a very nice tool to analyze source complexity.
		// - aournd 20,000 lines added.
		// - string_util is complex.
		// - watch more complex files and add more tests to cover more codes.
	}
}