#include "stdafx.h"
#include <catch.hpp>

struct kk
{

};

TEST_CASE("test hana")
{
	SECTION("understand hana")
	{

	}

	SECTION("type system with hana")
	{
		
	}

	SECTION("visual c++ typeid")
	{
		kk k;

		auto id = typeid(kk).name();

		//
		// �ٸ� �����Ϸ��� �𸣰����� 
		// copy constructor�� delete �Ǿ� �ִ�. 
		//
	}
}