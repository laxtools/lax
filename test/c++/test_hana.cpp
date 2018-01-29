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
		// 다른 컴파일러는 모르겠으나 
		// copy constructor가 delete 되어 있다. 
		//
	}
}