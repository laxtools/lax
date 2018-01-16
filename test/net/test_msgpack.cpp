#include "stdafx.h"
#include <catch.hpp>

namespace
{

class A
{
public: 
	static int v;

};

class B : public A
{

};

int A::v = 0;

} // noname


TEST_CASE("msgpack protocol")
{
	SECTION("c++ static variable")
	{
		A::v = 3;

		REQUIRE(B::v == 3);

		// to check protocol::channel_ is shared for all subclasses
	}

	SECTION("msgpack serialization performance")
	{
		// msgpack의 C++ 구현 내부를 본다. 
		// 
		// 얼마나 빠른 지, 개선할 점은 없는 지 살펴본다. 
	}
}