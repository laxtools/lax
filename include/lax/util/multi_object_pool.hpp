#pragma once 

namespace lax
{
namespace util
{

// 
// typeid�� ������� ���� object_pool�� ���� 
// Ǯ�� ã�Ƽ� �Ҵ� / ������ �Ѵ�. 
// actor, message�� ���� �پ��� ���� Ÿ���� ���� ��쿡 ����Ѵ�. 
//
// ���̵���̰� ���� ������ ���� ������� �Ѵ�. 
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
	// Ÿ�� �߷��� auto�� ���� ���������鼭 ���� ������ ���δ�. 
	// 
};

} // util 
} // lax