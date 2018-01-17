#pragma once 

namespace lax
{
namespace util
{

// 
// typeid를 기반으로 여러 object_pool을 갖고 
// 풀을 찾아서 할당 / 해제를 한다. 
// actor, message와 같이 다양한 하위 타잎을 갖는 경우에 사용한다. 
//
// 아이디어이고 구현 가능할 지는 살펴봐야 한다. 
class multi_object_pool
{
public: 
	multi_object_pool()
	{
	}

	// 
	// add_pool( typeid(T), pool*) 
	// templat <T>
	// construct() 
	//  find a pool with typeid(T) 
	//  call construct 
	// 
	// destroy() 
	//
	// 타잎 추론이 auto를 통해 가능해지면서 구현 가능해 보인다. 
	// 
};

} // util 
} // lax