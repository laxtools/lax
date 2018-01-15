#include "stdafx.h"
#include <catch.hpp>

#include <codecvt>
#include <string>

namespace unx
{
// utf8 version of conversion은 std::codecvt_*로 가능

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

// window에서 code page를 반영한 변환은 좀 그러하네.

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
			std::wstring ws(L"한글");

			std::string os;
			unx::convert(ws, os);

			// 여기서 변환된 문자열은 utf8임.

			std::wstring ow;
			unx::convert(os, ow);

			// 여기서는 제대로 변환된 것으로 보임.

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
			std::wstring ws(L"한글");

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
			std::wstring ws(L"한글");

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