#include "stdafx.h"
#include <catch.hpp>
#include <lax/util/string_util.hpp>
#include <iostream>

using namespace lax::util;

TEST_CASE("string util")
{
	SECTION("trim")
	{
		SECTION("standard")
		{
			std::string v{ " hello\t" };

			string_util::trim(v);

			REQUIRE(v == "hello");
		}

		SECTION("wide string")
		{
			std::wstring v{ L" hello\t" };

			string_util::trim(v);

			REQUIRE(v == L"hello");
		}
	}

	SECTION("convert")
	{
		SECTION("narrow to wide")
		{
			std::string ns{ "�ѱ�" };
			std::wstring os;

			string_util::convert(ns, os);
			REQUIRE(os == L"�ѱ�");
		}

		SECTION("wide to narrow")
		{
			std::wstring ns{ L"�ѱ�" };
			std::string os;

			string_util::convert(ns, os);
			REQUIRE(os == "�ѱ�");

			// std::cout << os << std::endl;
		}
	}

	SECTION("split")
	{
		std::string sv = "a;b;c:d,e:f";

		auto rv = string_util::tokenize(sv, ";,:");
		REQUIRE(rv.size() == 6);

		rv = string_util::tokenize(sv, ";,:", 3);
		REQUIRE(rv.size() == 4);
	}

	SECTION("tokenize")
	{
		std::wstring sv = L"abc, def, gh,";

		string_util::wvec vec; 

		string_util::tokenize(sv, vec, L",");

		REQUIRE(vec.size() == 4);
	}

	SECTION("replace_all")
	{
		std::string sv = "a;b;c:d,e;f";

		auto res = string_util::replace_all(sv, ";", "_");

		// �� ���ڿ��ε� �� �ٲ��.

		REQUIRE(res == "a_b_c:d,e_f");

		// std::cout << res << std::endl;
	}
}