#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>

using namespace lax::net;

TEST_CASE("service")
{
	SECTION("startup and shutdown")
	{
		service::cfg.use_hardware_concurreny = false;
		service::cfg.concurreny_level = 2;
		service::cfg.enable_detail_log = false;

		auto& svc = service::inst();

		// svc is a singleton, but it can be re-initialized after fini
		// 단위 테스트에 필요한 기능. 안정성 확보에 중요. 

		svc.init();

		svc.fini();

		// impl이 디버그에서 잘 안 보이는데 릴리스에서는 오히려 잘 보인다. 
		// - VS에서 일관되게 그러함 
		// - VS 포럼에 올라와 있다. 장비마다 다른 경우가 있다고 한다. 
		// - Visualizer가 제대로 동작하지 않는 문제인데 어딜 봐야할까?
		// - 일단, 유보
	}

	SECTION("acctor / connector / session ready")
	{
		auto& svc = service::inst();

		svc.init();

		svc.listen("127.0.0.1:7777", "bits");

		svc.connect("127.0.0.1:7777", "bits");

		svc.wait(1000);

		REQUIRE(svc.get_session_count() == 2);

		svc.fini();

		// 
		// 프로토콜 등록 안 된 상태면 exception이 발생.
		// - listen, connect에서 원천 봉쇄하고 로그 남김 
		// - 사전에 차단하는 게 가장 깔끔하다. 
		// - 크래시가 나거나 상태가 꼬이면 디버깅이 어려움 
		//
	}

	SECTION("ref and subscription")
	{
		//
		// sub for session ready and close 
		// - ready 
		// - get ref 
		//   - sub for close
		//   - handle close
		//
	}
}