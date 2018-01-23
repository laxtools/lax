#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include <lax/net/protocol/bits/bits_message_factory.hpp>

using namespace lax::net;

// to make this test readable (설계 작업 시 작성 파일) 
#include "bits_message_design.hpp"

namespace
{

// 자체 구조체 테스트 
struct item
{
	int id = 1; 
	int cost = 100;

	template <typename S>
	void serialize(S& s) 
	{
		// 편한 버전. 대부분의 경우 이렇게 하면 됨. 

		s.archive(
			id, 
			cost
		);
	}
};

// 최종 메세지 용례
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
		// 편한 버전. 대부분의 경우 이렇게 하면 됨. 

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
		// 최종 usage 정리. 메세지 정의와 사용이 편하다. 

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
		// 내부 구현을 설계 

		SECTION("send")
		{
			//
			// pack하고 길이를 제일 앞에 씀 
			// - pack에서 길이 필드 확보 
			// - 제일 앞에 쓰는 건 begin()을 사용 
			//

			// 이쪽은 bm_flex 테스트에서 확인 
		}

		SECTION("recv")
		{
			//
			// unpack()할 때 길이와 topic을 읽음
			// 

			BITS_MSG_REG(bm_flex);

			// send 
			auto mp = std::make_shared<bm_flex>();
			mp->name = "Hello";
			mp->items.push_back(item{ 1, 5 });

			resize_buffer buf;

			auto size = mp->pack(buf);
			REQUIRE(size > 0); // sie == 0이면 오류로 처리

			auto iter = buf.begin();
			iter;   *iter = size & 0x000000FF;
			++iter; *iter = size >> 8 & 0x000000FF;
			++iter; *iter = size >> 8 & 0x000000FF;
			++iter; *iter = size >> 8 & 0x000000FF;

			// recv	
			auto ri = buf.begin(); // current로 진행

			// 아래는 모두 little endian 구성에서 동작함. 
			// C++ shift operator는 모든 엔디안에서 동작

			// 길이 읽기 
			uint32_t len = 0;
			len  = *ri;				++ri;
			len |= (*ri << 8);		++ri;
			len |= (*ri << 16);		++ri;
			len |= (*ri << 24);		++ri;

			// 토픽 읽기 
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