#include "stdafx.h"
#include <catch.hpp>
#include <vector>
#include <iostream>

namespace
{

namespace
{

struct type
{

	using vec = std::vector<const std::type_info*>;

	vec types_;
};

struct object : public type
{
	object()
	{
		types_.push_back(&typeid(*this));
	}
};

struct object2 : public object
{
	object2()
	{
		types_.push_back(&typeid(*this));
	}
};

struct type_container
{
	void add(type* v) { objs_.push_back(v); }

	using vec = std::vector<type*>; // type *�� Ű�� �ϴ� ��� ���·� ����

	vec objs_;
};

} // anonymous

}

TEST_CASE("type system design - take 2")
{
	SECTION("test type basics")
	{
		object2 obj2;

		for (auto& tv : obj2.types_)
		{
			std::cout << tv->name() << std::endl; // namespace�� �����ϴ� �� �̸�. 
		}

		bool result = ((*obj2.types_[0]) == typeid(object)); // ������ ȣ�� ������ ����

		REQUIRE(result);
	}

	SECTION("type multiple inheritance")
	{
		// �����ϱⰡ ����� ���� ����. 
		// ���� ��� ������ �����Ѵ�. 
	}

	SECTION("type entry wrapper")
	{
		//
		// type_info*�� map �̳� unordered_map�� Ű�� 
		// ����ϴ� �� �����Ҹ� ���� �ʴ�. 
		// type_info�� �����ڸ� �巯���� adapter�� �ۼ��ؼ� ó�� 
		// 


	}
}

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

} // anonymous


TEST_CASE("type system design - take 1")
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
		// �߰�: 
		// - typeid()�� const std::type_info&�� �����ش�. 
		// - �� ������ �׻� ���� �ּҸ� ������ �����Ǿ���. 
		// - ���� ������ �����ϴ°� �� �� �����ϴ�. 
		// - ������ �� ������ ������ �ʴ´�. (Ȯ���� ����)

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

	SECTION("typeid and type_info")
	{
		REQUIRE(typeid(cls_base) == typeid(cls_derived::base));

		std::vector<const std::type_info*> types;

		types.push_back(&typeid(cls_derived));
		types.push_back(&typeid(cls_base));

		REQUIRE(types[0] == &typeid(cls_derived));
	}
}