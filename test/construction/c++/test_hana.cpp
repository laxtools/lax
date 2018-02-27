#include "stdafx.h"
#include <catch.hpp>


struct kk
{

};

TEST_CASE("test hana")
{
	// visual c++가 아직 c++14, c++17를 잘 지원하지 못 하고 있다. 슬픔이다. 
	// hana는 당분간 보류하고 typeid를 사용하는 방법을 찾는다. 

	// typeid의 결과 값을 포인터로 처리해서 constexpr const char* 와 
	// 같은 접근을 취할 수 있다. 좀 더 실수를 줄일 수 있는 방법. 
	// 여전히 완벽하지는 않다. 

	SECTION("visual c++ typeid")
	{
		const std::type_info& id = typeid(kk);

	}
}