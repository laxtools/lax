#include "stdafx.h"

#include <lax/actor/actor.h>
#include <catch.hpp>

using namespace lax::actor;
using namespace lax::net;

namespace
{

class dummy : public actor
{
public:
	dummy()
		: actor(33)
	{
	}

private:
	void on_process() override
	{
	}
};

}

TEST_CASE("actor send")
{

	SECTION("design")
	{
		// message 전송

		// 구분: 
		//  - client 
		//  - server
		//    - 역할별 구분

		// 범위: 
		//  - unicast 
		//  - multicast 
		//  - broadcast 
		
		// continuation
		//  - notify
		//  - request / response w/ timeout 

		// 에러 
		//  - disconntected 
		//  - shutdown 
		//  - ttl timeout

		// 참조 
		//  - session::ptr 참조 
		//  - 자동으로 바인딩 해제하는 기능 포함 
		//  

		SECTION("구현 액터")
		{
			// session_binder를 actor에 넣고 구현하고 
			// cluster_actor에서는 directory를 좀 더 활용한다. 

		}

		SECTION("범위")
		{
			
		}

		SECTION("continuation")
		{

		}

		SECTION("에러")
		{

		}
	}
	
}
