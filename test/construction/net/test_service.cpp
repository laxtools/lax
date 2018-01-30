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
		// ���� �׽�Ʈ�� �ʿ��� ���. ������ Ȯ���� �߿�. 

		svc.init();

		svc.fini();

		// impl�� ����׿��� �� �� ���̴µ� ������������ ������ �� ���δ�. 
		// - VS ������ �ö�� �ִ�. ��񸶴� �ٸ� ��찡 �ִٰ� �Ѵ�. 
		// - Visualizer�� ����� �������� �ʴ� �����ε� ��� �����ұ�?
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
			// ȣ���� ���Ƶ� �Լ� �����θ� �ؾ� �Ѵ�. 
			// �� �׷��� Ŀ�������� ���������� �Ұ����ϴ�. ( �Լ� ����!)
		}

		SECTION("test with a session")
		{
			auto& svc = service::inst();
		
			// lock�� �����Ƿ� ���� ȣ���� �� �ȴ�. �׽�Ʈ������ ����
			spdlog::set_level(spdlog::level::trace);

			svc.init();

			// ref ���� �� ���� �������� ��
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