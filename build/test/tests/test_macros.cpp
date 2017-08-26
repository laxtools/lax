#include "stdafx.h"

#include <lax/util/macros.h>

#include <catch.hpp>
#include <memory>
#include <iostream>
#include <map>

struct vtest
{
	int v; 

	virtual void hello()
	{
	}

	virtual void run()
	{

	}
};

struct aa
{
	aa()
		: val()
		, val2()
	{
	}

	uint32_t val;
	uint32_t val2 = uint32_t();
};


TEST_CASE("macros")
{
	SECTION("check")
	{
		// check(3 == 1);

		// check.log 파일 솔루션 폴더 (실행 폴더) 확인  
		// _DEBUG에서 assert() 호출 확인 
	}

	SECTION("return ")
	{
		// from variadic macros tricks 
		// visual c++ 140 still has macro expansion issue with variadic templates. 
#if 0

#define _MY_CONCAT3(a, b, c) a b c
#define _MY_CONCAT2(a, b) a b
#define _GET_OVERRIDE(_1, _2, _3, NAME, ...) NAME

#define MY_CONCAT(...) _GET_OVERRIDE(__VA_ARGS__, _MY_CONCAT3, _MY_CONCAT2)(__VA_ARGS__)

		std::cout << "3 args : " << MY_CONCAT("a", "b", "c") << std::endl;
#endif 
	}

	SECTION("return ")
	{
		return_if(!false);

		REQUIRE(false);
	}

}


int test_compile_return()
{
	return_if(false, 33);

	return 0;
}