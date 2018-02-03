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
}