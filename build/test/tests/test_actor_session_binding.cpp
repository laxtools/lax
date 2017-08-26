#include "stdafx.h"

#include <lax/actor/detail/session_binder.h>
#include <lax/net/protocol/json/session_json.h>
#include "test_app.h"
#include <catch.hpp>

using namespace lax::actor;
using namespace lax::net;

namespace
{
struct test_app : public lax::test::test_app 
{
	test_app(bool send)
		: send_(send)
	{
	}

	bool send_ = false;
};
} // noname

#include <lax/net/detail/service_impl.h>

TEST_CASE("actor session binding")
{
	// mockup이 어려워 send는 clsuter_actor / server 완료 후 테스트 진행 
	
	session_binder binder;

	test_app app(false);

	service svc(&app);
	tcp::socket soc(svc.impl()->get_ios());

	auto id = session::id(3, 0);
	auto tss = std::make_shared<session_json>(svc, id, std::move(soc), false);

	REQUIRE(binder.bind(tss));
	REQUIRE(binder.has(id));

	binder.unbind(id);

	REQUIRE(binder.has(id) == false);


	// TODO: 실질적인 연결을 통해 bind/unbind 테스트가 돼야 한다. 
	// 
}
