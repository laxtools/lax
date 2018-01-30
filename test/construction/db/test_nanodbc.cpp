#include "stdafx.h"
#include <catch.hpp>
#include <lax/db/nanodbc.h>

using namespace nanodbc;

// TEST_CASE("test nanodbc")
void TEST_NONE_NANODBC()
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

			// nanodbc를 수정
			//  - std::wstring 또는 std::string을 사용하도록 변경 
			//  - DB 결과 값에 대한 처리 확인
		}

		SECTION("dsn")
		{
			// odbc setup
			// sqlserver native client로 정상
			// odbc driver로 했을 경우 안 됨 
			SECTION("odbc driver")
			{
				connection c1;

				c1.connect(
					L"test_native",
					L"sa",
					L"nw4$anyAny",
					5
				);

				// ok

				c1.disconnect();
			}

			SECTION("direct execution")
			{
				connection c1;

				c1.connect(
					L"test_native",
					L"sa",
					L"nw4$anyAny",
					5
				);

				statement stmt;

				stmt.execute_direct(c1, L"select 1");

				c1.disconnect();
			}
		}
	}

	// queries work on  test_simple( id : unqiueidentifier, value : nvarchar(50)); 

	SECTION("statement")
	{
		SECTION("execute")
		{
			connection c1;

			c1.connect(
				L"test_native",
				L"sa",
				L"nw4$anyAny",
				5
			);

			//
			// 옵션 지정을 위해서는 DSN 문자열을 직접 사용하는 게 좋다. 
			// - database를 지정할 수 있는 유일한 방법이다.
			// - 아니면 datasource에서 지정해야 함

			statement stmt;

			stmt.open(c1);

			stmt.prepare(L"insert into test_simple values( NEWID(), 'd1')");

			auto result = stmt.execute();

			c1.disconnect();
		}

		SECTION("fetch")
		{
			connection c1;

			c1.connect(
				L"test_native",
				L"sa",
				L"nw4$anyAny",
				5
			);

			statement stmt;

			stmt.open(c1);

			stmt.prepare(L"select * from test_simple");

			auto result = stmt.execute();

			for (int i = 0; result.next(); ++i)
			{
				auto c1 = result.get<std::wstring>(0);
				auto c2 = result.get<std::wstring>(1);
			}

			// 한글 읽는데도 문제가 없다. 왜 동작하지?
			// - wstring을 사용하므로 convert를 실행하지 않음
			// - 따라서, 변환 없이 그대로 사용됨

			c1.disconnect();
		}

		SECTION("bind")
		{
			connection c1;

			c1.connect(
				L"test_native",
				L"sa",
				L"nw4$anyAny",
				5
			);

			statement stmt;

			stmt.open(c1);

			stmt.prepare(L"select * from test_simple where value = ?");
			stmt.bind(0, L"d1");

			auto result = stmt.execute();

			for (int i = 0; result.next(); ++i)
			{
				auto c1 = result.get<std::wstring>(0);
				auto c2 = result.get<std::wstring>(1);
			}

			// 한글 읽는데도 문제가 없다. 왜 동작하지?
			// - wstring을 사용하므로 convert를 실행하지 않음
			// - 따라서, 변환 없이 그대로 사용됨

			c1.disconnect();
		}
	}

	SECTION("transaction")
	{
		SECTION("commit level")
		{
		}


	}

	SECTION("error handling")
	{

	}
}