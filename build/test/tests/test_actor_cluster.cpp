#include "stdafx.h"
#include <lax/actor/server.h>
#include <lax/util/simple_timer.h>
#include <catch.hpp>

using namespace lax::actor;
using namespace lax::util;
using namespace json;

TEST_CASE("actor clustering")
{
	SECTION("cluster")
	{
		SECTION("startup / shutdown")
		{
			server_config cfg1; 
			cfg1.cluster_listen_addr = "127.0.0.1:7001";
			cfg1.cluster_peers.clear(); // 단방향으로만 연결을 맺는다. 

			server_config cfg2; 
			cfg2.cluster_listen_addr = "127.0.0.1:8001";
			cfg2.cluster_peers = std::vector<std::string>{ "127.0.0.1:7001" };

			simple_timer timer;

			server s1(id(3, 1));
			server s2(id(4, 1));

			// connect each other and exchanges refs 

			s1.start(cfg1);
			s2.start(cfg2);

			s1.process(timer.elapsed());
			s2.process(timer.elapsed());

			s1.stop();
			s2.stop();

			// unit test 구성으로 기능 테스트가 되도록 한다.
			// 중요한 과제 중 하나다.
		}
	}

	SECTION("exceptions")
	{
		// disconnect

		// timeout 

		// shutdown 

		// notify to unbind actor / sessions 

		// 
	}

}