#include "stdafx.h"

#include <catch.hpp>

TEST_CASE("design convention")
{
	SECTION("naming")
	{
		// asio convention

		// spdlog, caf has similar one

		// trailing underscore for member variables
	}

	SECTION("use const when possible")
	{
		// use mutable when required 
	}

	SECTION("use reference when possible")
	{
	}

	SECTION("use smart pointers except extremely unproper cases")
	{
	}

	SECTION("use error class (type) to report errors")
	{

	}

	SECTION("use exception only when essential")
	{
		// �ʼ����� ���� : 
		//   - ���� ��...
	}

	SECTION("use ppltasks.h writing flow")
	{
		// �ּ�. Ÿ�� ��� (type safety) 
		// naming. 
		// 
	}

}

