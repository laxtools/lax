#include "stdafx.h"
#include <catch.hpp>
#include <lax/actor/detail/type_info.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include "xxhash.h"

using namespace lax::actor;
using namespace lax::net;

namespace
{


struct item
{
	int id = 1;
	int cost = 100;

	BITS_SERIALIZE(
		id,
		cost
	);
};

struct bm_flex: public bits_message
{
	BITS_MSG(
		3, 
		5,
		bm_flex// class 
	);

	int key = 3;
	std::vector<item> items;

	BITS_SERIALIZE(
		key, 
		items
	);
};


struct msg
{
	int v = 0; 
	std::string name = "Hello";
};

} // noname

TEST_CASE("test type and content based dispatching")
{
	// design test. experimental 

	// �����丮�� ���ܵд�. ������Ʈ������ ���� 
	// �ؽô� ������ ���δ�. xxhash �̸��� ����ص�

	SECTION("hash generation")
	{
		bm_flex flex1; 

		resize_buffer buf; 

		flex1.pack(buf);

		auto hval1 = XXH32(buf.data(), buf.size(), 128);
	
		bm_flex flex2;

		flex2.key = 10;

		buf.rewind();

		flex2.pack(buf);

		auto hval2 = XXH32(buf.data(), buf.size(), 128);

		std::cout << hval1 << " : " << hval2 << std::endl;

		// pack �� �� �ʵ带 ������ �ʿ䰡 �ִ�. 
		// �پ��� �ʵ忡 ���� �� �񱳰� �ʿ�. 
		// ���� ������ ����ġ �ʴ�. 
		// ��뵵 �� c++ �������� �ʴ�. 
	}

	SECTION("equality operator")
	{

	}
}