#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include <lax/net/protocol/bits/bits_message_factory.hpp>

using namespace lax::net;

// to make this test readable (���� �۾� �� �ۼ� ����) 
#include "bits_message_design.hpp"

namespace
{

// ��ü ����ü �׽�Ʈ 
struct item
{
	int id = 1; 
	int cost = 100;

	template <typename S>
	void serialize(S& s) 
	{
		// ���� ����. ��κ��� ��� �̷��� �ϸ� ��. 

		s.archive(
			id, 
			cost
		);
	}
};

// ���� �޼��� ���
struct bm_flex : public bits_message
{
	BITS_MSG( 
		3,				// group
		4,				// type
		bm_flex			// class 
	);

	std::string name;
	int value = 33; 
	std::vector<item> items;

	template <typename S>
	void serialize(S& s) 
	{
		// ���� ����. ��κ��� ��� �̷��� �ϸ� ��. 

		s.archive(
			name, 
			value, 
			items
		);
	}
};



} // noname

TEST_CASE("bits protocol design")
{
	SECTION("flexible serialization")
	{
		// ���� usage ����. �޼��� ���ǿ� ����� ���ϴ�. 

		auto mp = std::make_shared<bm_flex>();
		mp->name = "Hello";
		mp->items.push_back(item{ 1, 5 });

		resize_buffer buf;

		auto size = mp->pack(buf);
		REQUIRE(size > 0);

		auto iter = buf.begin();
		iter;   *iter = size & 0x000000FF;
		++iter; *iter = size >> 8 & 0x000000FF;
		++iter; *iter = size >> 8 & 0x000000FF;
		++iter; *iter = size >> 8 & 0x000000FF;

		auto res = std::make_shared<bm_flex>();
		auto rc = res->unpack(buf, buf.begin(), size);

		REQUIRE(res->name == mp->name);
		REQUIRE(res->items[0].id == 1);
	}

	SECTION("protocol design")
	{
		// ���� ������ ���� 

		SECTION("send")
		{
			//
			// pack�ϰ� ���̸� ���� �տ� �� 
			// - pack���� ���� �ʵ� Ȯ�� 
			// - ���� �տ� ���� �� begin()�� ��� 
			//

			// ������ bm_flex �׽�Ʈ���� Ȯ�� 
		}

		SECTION("recv")
		{
			//
			// unpack()�� �� ���̿� topic�� ����
			// 

			BITS_MSG_REG(bm_flex);

			// send 
			auto mp = std::make_shared<bm_flex>();
			mp->name = "Hello";
			mp->items.push_back(item{ 1, 5 });

			resize_buffer buf;

			auto size = mp->pack(buf);
			REQUIRE(size > 0); // sie == 0�̸� ������ ó��

			auto iter = buf.begin();
			iter;   *iter = size & 0x000000FF;
			++iter; *iter = size >> 8 & 0x000000FF;
			++iter; *iter = size >> 8 & 0x000000FF;
			++iter; *iter = size >> 8 & 0x000000FF;

			// recv	
			auto ri = buf.begin(); // current�� ����

			// �Ʒ��� ��� little endian �������� ������. 
			// C++ shift operator�� ��� ����ȿ��� ����

			// ���� �б� 
			uint32_t len = 0;
			len  = *ri;				++ri;
			len |= (*ri << 8);		++ri;
			len |= (*ri << 16);		++ri;
			len |= (*ri << 24);		++ri;

			// ���� �б� 
			uint32_t topic = 0;
			topic = *ri;				++ri;
			topic |= (*ri << 8);		++ri;
			topic |= (*ri << 16);		++ri;
			topic |= (*ri << 24);		++ri;

			auto res = BITS_MSG_CREATE(topic);
			auto rc = res->unpack(buf, buf.begin(), size);

			auto mm = std::static_pointer_cast<bm_flex>(res);

			REQUIRE(mm->name == mp->name);
			REQUIRE(mm->items[0].id == 1);
		}

		SECTION("cipher")
		{

		}

		SECTION("checksum")
		{

		}
	}
}