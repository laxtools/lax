#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include <lax/net/detail/subs_close.hpp>
#include <lax/net/protocol/sys/sys_messages.hpp>
#include <lax/util/logger.hpp>

using namespace lax::net;
using namespace lax::channel;

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
		// - VS 포럼에 올라와 있다. 장비마다 다른 경우가 있다고 한다. 
		// - Visualizer가 제대로 동작하지 않는 문제인데 어딜 봐야할까?
	}

	SECTION("acctor / connector")
	{
		auto& svc = service::inst();

		svc.init();

		svc.listen("127.0.0.1:7777", "bits");

		svc.connect("127.0.0.1:7777", "bits");

		svc.wait(1000);

		REQUIRE(svc.get_session_count() == 2);

		svc.fini();
	}

	SECTION("close subs")
	{
		SECTION("test subs_close class")
		{
			subs_close subs;

			auto key = subs.subscribe(3, [](packet::ptr m) {});
			subs.unsubscribe(key);

			key = subs.subscribe(5, [](packet::ptr m) {});
			REQUIRE(subs.post(5) == 1); // single entry

			auto key1 = subs.subscribe(5, [](packet::ptr m) {});
			auto key2 = subs.subscribe(5, [](packet::ptr m) {});
			REQUIRE(subs.post(5) == 2); // multiple entry

			REQUIRE(subs.post(5) == 0); // erased after post

			// 100% coverage without a tool 
			// 호출이 많아도 함수 단위로만 해야 한다. 
			// 콜 그래프 커버리지는 현실적으로 불가능하다. ( 함수 개수!)
		}

		SECTION("test with a session")
		{
			auto& svc = service::inst();
		
			// lock이 있으므로 자주 호출은 안 된다. 테스트에서는 가능
			spdlog::set_level(spdlog::level::trace);

			svc.init();

			// ref 참조 등 먼저 지워져야 함
			{
				std::vector<packet::sid_t> sids;

				// session_ready sub
				session::sub(
					sys_session_ready::get_topic(),
					[&sids](message::ptr m) {
					auto pp = std::static_pointer_cast<packet>(m);
					sids.push_back(pp->id);
				}
				);

				svc.listen("127.0.0.1:7777", "bits");

				svc.connect("127.0.0.1:7777", "bits");

				svc.wait(2000);

				//REQUIRE(svc.get_session_count() == 2);
				//REQUIRE(sids.size() == 2);

				// acquire ref 
				auto sref = svc.acquire(session::id(sids[0]));
				//REQUIRE(sref.is_valid());

				// sub for close
				int close_count = 0;

				// called from io thread
				sref.sub_close(
					[&sref, &close_count](message::ptr m) {
					auto pp = std::static_pointer_cast<sys_session_closed>(m);
					++close_count;
				}
				);

				// close the last session
				sref.close();

				svc.wait(2000);

				// check close callback 
				//REQUIRE(close_count == 1);
			}

			svc.fini();

			svc.wait(1000);

			spdlog::set_level(spdlog::level::info);
		}
	}
}