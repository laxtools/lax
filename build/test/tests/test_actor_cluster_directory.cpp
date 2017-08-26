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
			// ������ �ϸ� ����� ������ json data �޸𸮰� ���� 
			// �����ϸ� �޸𸮴� ������ json data ���� �� thread �̽� �߻� 

			// 
			// json �����͸� �����ؼ� ����ϵ��� �ϸ� ���? 
			// �ʿ��� ������ �����ϸ� �Ǳ� ������... 
			// 

			// 
			// �ʿ��� ������ ó���� �� �ֵ��� �Լ� �߰� 
			// proc_attributes() �Լ� override�ؼ� �ʿ��� �������� ���
			// 
		}
	}

	SECTION("reference")
	{
		// ������ ����� �����ϴ� ������ �������� 
		// Name, Location, Status ������ ���� ����
		// LDAP�� ���� ���� �뵵�� �� �ٸ�

		SECTION("DNS")
		{
			// hierarchical decentralized . --> ���� Ŭ�����͸��� ���� 
			// 
			// �ͼ������� ĳ�̰� TTL, authoritative server, reverse lookup ������ 
			// ���� ��� ���� �������� �����

			// caching: 
			//  - ������ ���̰� ������ �����Ϸ��� ���
			//  - �� �κ��� �߿�. 
			//  - ���� ���� ���͵��� �ִ��� ���� ������ 
			//    �ִ��� ��Ȯ�ϰ� ���θ� �˰� �ϴ� ��
		}

		SECTION("LDAP")
		{
			// �Ϲ����� ���� ���� ���� 
			// ���������δ� ���� ���������� ���

			// client connects to ldap server (called Directory System Agent) 
			// operations: 
			//	bind - authenticate
			//  search 
			//  compare - test for attribute for a given name
			//  add 
			//  delete 
			//  modify 
			//  unbind - close connection

			// �л� ó���� ���������� ������ �߾� ������ Ŭ���̾�Ʈ�� ���δ�. 
			// ���� �� �ʿ� �л� ���丮���� �ִ� ������� �л��� �����ȴ�. 
			
			// ������ : 
			// dn, cn, sn ������ �Ǵ� ������. 
			// 
			// �������� : 
			//  binary 
			//  dc (domain component)��� �Ҹ��� ���� ���� �Ӽ� ����
			// 

			// �⺻������ ��ȸ / ���� ����̶� ���� Ŭ�����͸� ���ٴ� 
			// ����� ���� ��ȸ � ����
			// ������ �׷� �����δ� key ����� ĳ�� ���񽺰� �� �����ϴ�.  
		}
	}
}