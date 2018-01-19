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
		// ���� �׽�Ʈ�� �ʿ��� ���. ������ Ȯ���� �߿�. 

		svc.init();

		svc.fini();

		// impl�� ����׿��� �� �� ���̴µ� ������������ ������ �� ���δ�. 
		// - VS���� �ϰ��ǰ� �׷��� 
		// - VS ������ �ö�� �ִ�. ��񸶴� �ٸ� ��찡 �ִٰ� �Ѵ�. 
		// - Visualizer�� ����� �������� �ʴ� �����ε� ��� �����ұ�?
		// - �ϴ�, ����
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
		SECTION("test close_subs class")
		{

		}

		SECTION("test with a session")
		{

		}
	}

	SECTION("ready subs")
	{
		// ready 

		// close 

		// callback
	}
}