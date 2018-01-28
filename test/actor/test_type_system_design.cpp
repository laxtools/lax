#include "stdafx.h"
#include <catch.hpp>

namespace
{

struct cls_base
{
	using base = cls_base; 
	static constexpr char* base_name = "none";
	static constexpr char* cls_name = "cls_base";

};

struct cls_derived : public cls_base
{
	using base = cls_base; 
	static constexpr char* base_name = "cls_base";
	static constexpr char* cls_name = "cls_derived";
};

}

TEST_CASE("type system design")
{
	SECTION("conclusion and summary")
	{
		//
		// �Ʒ����� �� ���� Ÿ�� �ý����� ���� ���̴�. 
		// �ϴ� ���� �ʿ��� �������� �����Ѵ�. 
		// - ���� ��� ���� ������ �Ѵ�. 
		// - constexpr�� char* �ּ� ������� �Ѵ�. 
		// - ���� Ŭ������ ����Ѵ�. 
		// - Ŭ������ ã�� �� ������ is_a()�� �˻��Ѵ�. 
		//

		// 
		// ������ ������ ����. 
		// - ���� ��� ������ �ִ� �� ���� ������Ʈ�� �� ���Ϳ��� ���� ����� �� ����. 
		// - ������ ���� ��ȸ�Ѵ�. 
		//   - ������ ���� adapter component�� ����� ����ϰ� ����Ѵ�. 
		//   - �ʿ�� �ش� adapter�� �������� ���� �������� �Ѵ�.
		//

		//
		// ���� ������ component�� actor������ �����ϴ�. 
		// �ٸ� ���� interface�� ������ ����, �ʿ�� ĳ������ ���� ó���Ѵ�.
		//
	}


	SECTION("constexpr char* idea")
	{
		// static constexpr type_t type = "component";
		// ������ �������� ��������� ������ �־� ���δ�. 
		// ���Ϸ��� ���� ��Ȯ�ؾ� �Ѵ�. 
		// ���� ������ ���� ������ ������ Ȯ������ �ʴ�. 

		REQUIRE(cls_derived::cls_name != cls_base::cls_name);
		REQUIRE(cls_derived::base_name == cls_base::cls_name);

		// constexpr char*�� visual c++���� ������ �ּҸ� �Ҵ��Ѵ�.
		// - �������� ����� ��� �����Ѵ�.
		// ��κ��� �����Ϸ� ������ �̸� ���� ������ ���δ�. 
		// Ȯ���� �ʿ������� ���� ������ �񱳷� ������ 
		// Ÿ�� �ý����� ������ �� �־� ���δ�.

		//
		// base_name = cls_base::cls_name���� �����ϸ� �� �������̴�. 
		// 
	}

	SECTION("c++ type system")
	{
		// rtti�� Ȱ��ȭ�ϸ� typeid�� ����. 
		// - ��ũ�η� Ȱ��ȭ�� 
		// - rtti�� Ȱ��ȭ ��Ű�� ĳ������ ���������� �� ������ 
		//   ������ �� ����� �������� �ʴ´�. 
		// - ���� ������ �����鼭 ������ type ��� ����� �ʿ��ϴ�. 
		// - �̺��� �����ϰ� �����ϸ� ���� �ý����� �ʿ�
	}

	SECTION("separate registration based")
	{
		// how can we can unique value for each type?	
		// - what can be the value?
		// - enum?
		
		/**
		   actor 
			  lobby 
			     poker9_lobby 
			  character 
			     pc 
				 npc 
				   boss
		 */

		//
		// a separate parser can generate static type information. 
		// 
		// Unreal 4 uses this approach.
		// - a separate compiler based reflection / type introspection system
		//
	}

}