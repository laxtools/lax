#include "stdafx.h"
#include <catch.hpp>
#include <lax/util/logger.hpp>
#include <iostream>

using namespace lax::util;

TEST_CASE("logger")
{
	SECTION("basics")
	{
		log::get()->info("hello {0}", "abc");
	}

	SECTION("wchar utf8 support")
	{
		// spdlog의 tweakme.h에 WIDE to UTF8 지원을 켜면 UTF8 문자열로 출력됨 
		// 콘솔은 당연히 보이지 않고 파일 로그에서는 잘 보임
		log::get()->info(L"hello {0}", L"한글 문자");
	}
}