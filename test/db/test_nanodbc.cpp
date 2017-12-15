#include "stdafx.h"
#include <catch.hpp>
#include <lax/db/nanodbc.h>

using namespace nanodbc;

TEST_CASE("test nanodbc")
{
	// 개념들: 
	//  - connection 
	//  - statement 
	//  - result 
	//  - exception 
	//
	// statement와 result가 핵심 
	// 에러 처리는 exception으로 진행

	// DB setup 
	//  - SQLExpress 
	//  - Free
	//  - 4 core, 1GB memory, 10GB DB
	//  - 테스트용, 작은 서비스 용도


	SECTION("connection")
	{
		// odbc setup 
		// - DSN (system)

		SECTION("string type")
		{
			connection c1;

			CHECK_THROWS(
				c1.connect(L"test", L"sa", L"sa", 5)
			);

			// string_type과 visual c++ 프로젝트의 와이드 버전 함수들 사이에 충돌이 발생
			//
			// std::string을 사용해서 빌드 
			// - API의 W 버전을 사용 못 해서 오류  
			// 
			// std::u32string을 std::wstring으로 변경하고 테스트 
			//  - convert 함수에서 빌드 에러 발생
			//

			// std::u32string으로 빌드 
			// - 위의 connect 함수에서 char[4]에서 std::u32string으로 변환 오류 발생 
			// - U 추가 
			//   - convert 링크 에러
			//   - std::codecvt<char32_t,char,struct _Mbstatet>::id"
			//   - VSO#143857 : Visual C++ 버그.... ㅠㅠ
			// 
			//   - 수정: typedef __int32 wchar_b_t
			//     - 일단 링크는 되는데 동작 여부는 확인해야 한다. 
			//     - 아마도 되지 않을까 싶은데 테스트 필요

			// 
			// https://stackoverflow.com/questions/31302506/stdu32string-conversion-to-from-stdstring-and-stdu16string
			// - std::wstring_convert 함수 사용법 
			// - std::codecvt 함수 사용법
		}

		SECTION("string handling. i18n")
		{
			// 저장 타잎: 
			//  - 코드 : std::wstring --> nanodbc는 std::u16string 또는 std::u32string을 써야 함
			//  - DB :  
			//  - 데이터 :  

			// odbc: 
			//  - ??


			// 문제: 
			//  - 윈도우는 std::wstring이 다국어 지원에 편리 
			//    - 데이터 포맷으로도 std::wstring이 편리 
			//    - 어떻게 할 것인가? 

			// Unreal 4의 접근: 
			//  - 코드 / 저장소 : std::wstring을 사용 
			//  - 필요한 곳에서는 변환해서 사용 
			//  

			// nanodbc를 재작성 
			//  - std::wstring 또는 std::string을 사용하도록 변경 
			//  - DB 결과 값에 대한 처리 확인
		}
	}

	SECTION("statement")
	{
		// 성능과 안정성 테스트를 충분히 많이해야 한다. 
	}

	SECTION("unicode")
	{
		// https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
		// wstring <--> string conversion

		
	}

	SECTION("transaction")
	{
	}

	SECTION("error handling")
	{
	}

	SECTION("performance")
	{
		SECTION("populate")
		{
		}

		SECTION("query")
		{
		}

		SECTION("delete")
		{
		}
	}
}