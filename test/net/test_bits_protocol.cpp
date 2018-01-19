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
		// 메세지 정의와 등록
		// 

		//
		// pack / unpack 테스트 
		// 
	}

	SECTION("communication w/ lambda callbacks")
	{
		auto& svc = service::inst();

		svc.init();



		svc.fini();
	}
}