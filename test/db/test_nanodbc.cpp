#include "stdafx.h"
#include <catch.hpp>
#include <lax/db/nanodbc.h>

using namespace nanodbc;

TEST_CASE("test nanodbc")
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

			// nanodbc�� ���ۼ� 
			//  - std::wstring �Ǵ� std::string�� ����ϵ��� ���� 
			//  - DB ��� ���� ���� ó�� Ȯ��
		}
	}

	SECTION("statement")
	{
		// ���ɰ� ������ �׽�Ʈ�� ����� �����ؾ� �Ѵ�. 
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