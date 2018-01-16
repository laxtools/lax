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
		// msgpack�� C++ ���� ���θ� ����. 
		// 
		// �󸶳� ���� ��, ������ ���� ���� �� ���캻��. 
	}
}