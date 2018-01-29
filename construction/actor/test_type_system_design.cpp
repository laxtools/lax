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

	using vec = std::vector<type*>; // type *를 키로 하는 목록 형태로 변경

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
			std::cout << tv->name() << std::endl; // namespace를 포함하는 긴 이름. 
		}

		bool result = ((*obj2.types_[0]) == typeid(object)); // 생성자 호출 순서에 의존

		REQUIRE(result);
	}

	SECTION("type multiple inheritance")
	{
		// 지원하기가 곤란한 점이 많다. 
		// 단일 상속 계층만 지원한다. 
	}

	SECTION("type entry wrapper")
	{
		//
		// type_info*로 map 이나 unordered_map의 키로 
		// 사용하는 건 권장할만 하지 않다. 
		// type_info의 연산자를 드러내는 adapter를 작성해서 처리 
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
		// 아래보다 더 나은 타잎 시스템은 있을 것이다. 
		// 일단 현재 필요한 수준으로 정리한다. 
		// - 단일 상속 계층 구조로 한다. 
		// - constexpr의 char* 주소 기반으로 한다. 
		// - 최종 클래스를 등록한다. 
		// - 클래스를 찾을 때 없으면 is_a()로 검색한다. 
		//
		// 추가: 
		// - typeid()가 const std::type_info&를 돌려준다. 
		// - 위 참조는 항상 같은 주소를 같도록 구현되었다. 
		// - 위의 구현에 의존하는게 좀 더 안전하다. 
		// - 여전히 쏙 마음에 들지는 않는다. (확신이 없다)

		// 
		// 제약은 다음과 같다. 
		// - 같은 상속 계층에 있는 두 개의 컴포넌트를 한 액터에서 같이 사용할 수 없다. 
		// - 다음과 같이 우회한다. 
		//   - 포함을 통해 adapter component를 만들어 등록하고 사용한다. 
		//   - 필요시 해당 adapter를 다형성의 시작 지점으로 한다.
		//

		//
		// 위의 구조는 component와 actor에서만 유용하다. 
		// 다른 경우는 interface와 정적인 포함, 필요시 캐스팅을 통해 처리한다.
		//
	}


	SECTION("constexpr char* idea")
	{
		// static constexpr type_t type = "component";
		// 괜찮아 보이지만 언어적으로 문제가 있어 보인다. 
		// 뭘하려는 건지 명확해야 한다. 
		// 뭔가 괜찮은 감은 있지만 동작이 확실하지 않다. 

		REQUIRE(cls_derived::cls_name != cls_base::cls_name);
		REQUIRE(cls_derived::base_name == cls_base::cls_name);

		// constexpr char*이 visual c++에서 고유한 주소를 할당한다.
		// - 릴리스와 디버그 모두 동작한다.
		// 대부분의 컴파일러 구현이 이를 따를 것으로 보인다. 
		// 확인은 필요하지만 빠른 포인터 비교로 간단한 
		// 타잎 시스템을 구현할 수 있어 보인다.

		//
		// base_name = cls_base::cls_name으로 구현하면 더 안정적이다. 
		// 
	}

	SECTION("c++ type system")
	{
		// rtti를 활성화하면 typeid가 동작. 
		// - 매크로로 활성화됨 
		// - rtti를 활성화 시키면 캐스팅이 가능하지만 형 쿼리나 
		//   안전한 형 사용을 보장하지 않는다. 
		// - 뭔가 가볍고 빠르면서 안전한 type 사용 방법이 필요하다. 
		// - 이보다 안전하고 완전하며 빠른 시스템이 필요
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