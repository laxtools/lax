#include "stdafx.h"
#include <catch.hpp>

#include <codecvt>
#include <string>

namespace unx
{
// utf8 version of conversion�� std::codecvt_*�� ����

inline void convert(const std::wstring& in, std::string& out)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	out = converter.to_bytes(in);
}

inline void convert(const std::string& in, std::wstring& out)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	out = converter.from_bytes(in);
}
} // unx

namespace win
{

// window���� code page�� �ݿ��� ��ȯ�� �� �׷��ϳ�.

inline void convert(const std::wstring& in, std::string& out)
{
	std::string str(in.size(), 0);

	auto& ctype = std::use_facet<std::ctype<wchar_t> >(std::locale());

	const char rep = '_';
	ctype.narrow(&in[0], &in[0] + in.size(), rep, &str[0]);

	out = str;
}

inline void convert(const std::string& in, std::wstring& out)
{
	std::wstring wstr(in.size(), 0);
	auto& ctype = std::use_facet<std::ctype<wchar_t>>(std::locale());

	ctype.widen(
		&in[0],
		&in[0] + in.size(),
		&wstr[0]
	);
	out = wstr;
}
} // win


namespace win2
{

// c style code page based conversion

inline void convert(const std::wstring& in, std::string& out)
{
}

inline void convert(const std::string& in, std::wstring& out)
{
}

} // win2

TEST_NONE("test string conversion")
{
	SECTION("unx")
	{
		SECTION("korean")
		{
			std::wstring ws(L"�ѱ�");

			std::string os;
			unx::convert(ws, os);

			// ���⼭ ��ȯ�� ���ڿ��� utf8��.

			std::wstring ow;
			unx::convert(os, ow);

			// ���⼭�� ����� ��ȯ�� ������ ����.

			REQUIRE(ow == ws);
		}

		SECTION("english")
		{
			std::wstring ws(L"hello");

			std::string os;
			unx::convert(ws, os);

			std::wstring ow;
			unx::convert(os, ow);

			//REQUIRE(ow == ws);
		}
	}

	SECTION("win")
	{
		SECTION("korean")
		{
			std::wstring ws(L"�ѱ�");

			std::string os;
			win::convert(ws, os);

			std::wstring ow;
			win::convert(os, ow);

			//REQUIRE(ow == ws);
		}

		SECTION("english")
		{
			std::wstring ws(L"hello");

			std::string os;
			win::convert(ws, os);

			std::wstring ow;
			win::convert(os, ow);

			//REQUIRE(ow == ws);
		}
	}

	SECTION("win2")
	{
		SECTION("korean")
		{
			std::wstring ws(L"�ѱ�");

			std::string os;
			win2::convert(ws, os);

			std::wstring ow;
			win2::convert(os, ow);

			//REQUIRE(ow == ws);
		}

		SECTION("english")
		{
			std::wstring ws(L"hello");

			std::string os;
			win2::convert(ws, os);

			std::wstring ow;
			win2::convert(os, ow);

			//REQUIRE(ow == ws);
		}
	}
}