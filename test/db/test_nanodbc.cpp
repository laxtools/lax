#include "stdafx.h"
#include <catch.hpp>
#include <lax/db/nanodbc.h>

using namespace nanodbc;

// TEST_CASE("test nanodbc")
void TEST_NONE_NANODBC()
{
	// �����: 
	//  - connection 
	//  - statement 
	//  - result 
	//  - exception 
	//
	// statement�� result�� �ٽ� 
	// ���� ó���� exception���� ����

	// DB setup 
	//  - SQLExpress 
	//  - Free
	//  - 4 core, 1GB memory, 10GB DB
	//  - �׽�Ʈ��, ���� ���� �뵵


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

			// string_type�� visual c++ ������Ʈ�� ���̵� ���� �Լ��� ���̿� �浹�� �߻�
			//
			// std::string�� ����ؼ� ���� 
			// - API�� W ������ ��� �� �ؼ� ����  
			// 
			// std::u32string�� std::wstring���� �����ϰ� �׽�Ʈ 
			//  - convert �Լ����� ���� ���� �߻�
			//

			// std::u32string���� ���� 
			// - ���� connect �Լ����� char[4]���� std::u32string���� ��ȯ ���� �߻� 
			// - U �߰� 
			//   - convert ��ũ ����
			//   - std::codecvt<char32_t,char,struct _Mbstatet>::id"
			//   - VSO#143857 : Visual C++ ����.... �Ф�
			// 
			//   - ����: typedef __int32 wchar_b_t
			//     - �ϴ� ��ũ�� �Ǵµ� ���� ���δ� Ȯ���ؾ� �Ѵ�. 
			//     - �Ƹ��� ���� ������ ������ �׽�Ʈ �ʿ�

			// 
			// https://stackoverflow.com/questions/31302506/stdu32string-conversion-to-from-stdstring-and-stdu16string
			// - std::wstring_convert �Լ� ���� 
			// - std::codecvt �Լ� ����
		}

		SECTION("string handling. i18n")
		{
			// ���� Ÿ��: 
			//  - �ڵ� : std::wstring --> nanodbc�� std::u16string �Ǵ� std::u32string�� ��� ��
			//  - DB :  
			//  - ������ :  

			// odbc: 
			//  - ??


			// ����: 
			//  - ������� std::wstring�� �ٱ��� ������ �� 
			//    - ������ �������ε� std::wstring�� �� 
			//    - ��� �� ���ΰ�? 

			// Unreal 4�� ����: 
			//  - �ڵ� / ����� : std::wstring�� ��� 
			//  - �ʿ��� �������� ��ȯ�ؼ� ��� 
			//  

			// nanodbc�� ����
			//  - std::wstring �Ǵ� std::string�� ����ϵ��� ���� 
			//  - DB ��� ���� ���� ó�� Ȯ��
		}

		SECTION("dsn")
		{
			// odbc setup
			// sqlserver native client�� ����
			// odbc driver�� ���� ��� �� �� 
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
			// �ɼ� ������ ���ؼ��� DSN ���ڿ��� ���� ����ϴ� �� ����. 
			// - database�� ������ �� �ִ� ������ ����̴�.
			// - �ƴϸ� datasource���� �����ؾ� ��

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

			// �ѱ� �дµ��� ������ ����. �� ��������?
			// - wstring�� ����ϹǷ� convert�� �������� ����
			// - ����, ��ȯ ���� �״�� ����

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

			// �ѱ� �дµ��� ������ ����. �� ��������?
			// - wstring�� ����ϹǷ� convert�� �������� ����
			// - ����, ��ȯ ���� �״�� ����

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