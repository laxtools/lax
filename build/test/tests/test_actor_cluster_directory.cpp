#include "stdafx.h"

#include <lax/actor/detail/directory.h>
#include <lax/net/session.h>
#include <catch.hpp>

using namespace lax::actor;
using namespace lax::net;

TEST_CASE("actor cluster dictionary")
{

	SECTION("usage")
	{
		directory dir; 

		id aid;

		aid.node = 3;
		aid.local = 5;

		ref::ptr ptr = std::make_shared<ref>(aid);

		ptr->update(3, session::id(3, 33), 11, ref::state::up);

		
		REQUIRE(dir.update(ptr));
		REQUIRE(dir.find(aid));

		auto refs = dir.find_role(11);
		REQUIRE(!refs.empty());
		REQUIRE(refs[0]->get_id().local == 5);
	}

	SECTION("design")
	{
		SECTION("directory design")
		{
			// 
			// data: 
			//  - dict<id, ref> 
			//  - index<role, ids> 
			//  
		}

		SECTION("memory / thread safety balance")
		{
			// 
			// 복제를 하면 사용이 쉽지만 json data 메모리가 증가 
			// 공유하면 메모리는 적지만 json data 접근 시 thread 이슈 발생 

			// 
			// json 데이터만 복사해서 사용하도록 하면 어떤가? 
			// 필요한 곳에서 보관하면 되긴 하지만... 
			// 

			// 
			// 필요한 곳에서 처리할 수 있도록 함수 추가 
			// proc_attributes() 함수 override해서 필요한 곳에서만 사용
			// 
		}
	}

	SECTION("reference")
	{
		// 유사한 기능을 제공하는 오래된 프로토콜 
		// Name, Location, Status 관리로 거의 동일
		// LDAP은 정보 관리 용도라 좀 다름

		SECTION("DNS")
		{
			// hierarchical decentralized . --> 서버 클러스터링에 적합 
			// 
			// 익숙하지만 캐싱과 TTL, authoritative server, reverse lookup 등으로 
			// 실제 운영해 보면 생각보다 어려움

			// caching: 
			//  - 전송을 줄이고 정보를 유지하려는 노력
			//  - 이 부분이 중요. 
			//  - 아주 많은 액터들이 있더라도 적은 정보로 
			//    최대한 정확하게 서로를 알게 하는 것
		}

		SECTION("LDAP")
		{
			// 일반적인 계층 정보 관리 
			// 실질적으로는 통합 인증용으로 사용

			// client connects to ldap server (called Directory System Agent) 
			// operations: 
			//	bind - authenticate
			//  search 
			//  compare - test for attribute for a given name
			//  add 
			//  delete 
			//  modify 
			//  unbind - close connection

			// 분산 처리가 가능하지만 서버는 중앙 서버로 클라이언트에 보인다. 
			// 서버 뒤 쪽에 분산 스토리지가 있는 방식으로 분산이 구현된다. 
			
			// 데이터 : 
			// dn, cn, sn 등으로 되는 정보들. 
			// 
			// 프로토콜 : 
			//  binary 
			//  dc (domain component)라고 불리는 계층 구성 속성 제공
			// 

			// 기본적으로 조회 / 응답 방식이라 게임 클러스터링 보다는 
			// 사용자 정보 조회 등에 적합
			// 하지만 그런 쪽으로는 key 기반의 캐시 서비스가 더 적합하다.  
		}
	}
}