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
		// spdlog�� tweakme.h�� WIDE to UTF8 ������ �Ѹ� UTF8 ���ڿ��� ��µ� 
		// �ܼ��� �翬�� ������ �ʰ� ���� �α׿����� �� ����
		log::get()->info(L"hello {0}", L"�ѱ� ����");
	}
}