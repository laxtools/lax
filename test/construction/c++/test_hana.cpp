#include "stdafx.h"
#include <catch.hpp>


struct kk
{

};

TEST_CASE("test hana")
{
	// visual c++�� ���� c++14, c++17�� �� �������� �� �ϰ� �ִ�. �����̴�. 
	// hana�� ��а� �����ϰ� typeid�� ����ϴ� ����� ã�´�. 

	// typeid�� ��� ���� �����ͷ� ó���ؼ� constexpr const char* �� 
	// ���� ������ ���� �� �ִ�. �� �� �Ǽ��� ���� �� �ִ� ���. 
	// ������ �Ϻ������� �ʴ�. 

	SECTION("visual c++ typeid")
	{
		const std::type_info& id = typeid(kk);

	}
}