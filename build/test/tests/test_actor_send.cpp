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
		// message ����

		// ����: 
		//  - client 
		//  - server
		//    - ���Һ� ����

		// ����: 
		//  - unicast 
		//  - multicast 
		//  - broadcast 
		
		// continuation
		//  - notify
		//  - request / response w/ timeout 

		// ���� 
		//  - disconntected 
		//  - shutdown 
		//  - ttl timeout

		// ���� 
		//  - session::ptr ���� 
		//  - �ڵ����� ���ε� �����ϴ� ��� ���� 
		//  

		SECTION("���� ����")
		{
			// session_binder�� actor�� �ְ� �����ϰ� 
			// cluster_actor������ directory�� �� �� Ȱ���Ѵ�. 

		}

		SECTION("����")
		{
			
		}

		SECTION("continuation")
		{

		}

		SECTION("����")
		{

		}
	}
	
}
