#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>

using namespace lax::net;

TEST_CASE("bits protocol")
{
	SECTION("adding messages")
	{
		//
		// �޼��� ���ǿ� ���
		// 

		//
		// pack / unpack �׽�Ʈ 
		// 
	}

	SECTION("communication w/ lambda callbacks")
	{
		auto& svc = service::inst();

		svc.init();



		svc.fini();
	}
}