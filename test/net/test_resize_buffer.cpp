#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/detail/buffer/resize_buffer.hpp>
#include <lax/util/simple_timer.hpp>
#include <iostream>

using namespace lax::net;

TEST_CASE("recv buffer")
{
	SECTION("basic usage")
	{
		resize_buffer rb; 

		REQUIRE(rb.append("abc", 3) == 3);
		REQUIRE(rb.data()[0] == 'a');
	}

	SECTION("resizing")
	{
		resize_buffer rb(1024); 

		for (int i = 0; i < 1024; ++i)
		{
			rb.append("a", 1);
		}

		REQUIRE(rb.capacity() == 1024);
		REQUIRE(rb.get_pool(1024)->get_stat().alloc_count == 1);
	}

	SECTION("pop front")
	{
		resize_buffer rb; 

		REQUIRE(rb.append("abc", 3) == 3);
		REQUIRE(rb.data()[0] == 'a');

		rb.pop_front(2); 

		REQUIRE(rb.data()[0] == 'c');
	}

	SECTION("alloc from os")
	{
		multiple_size_buffer_pool pool;

		auto buf = pool.alloc(1024 * 1024 * 10 );

		REQUIRE(buf->is_allocated_from_os());
		REQUIRE(buf->capacity() == 1024 * 1024 * 10);

		pool.release(buf);
	}

	SECTION("simple performance")
	{
		constexpr int test_count = 10000;

		lax::util::simple_timer timer;

		for (int i = 0; i < test_count; ++i)
		{
			resize_buffer rb(1024*8);

			for (int k = 0; k < 16; ++k)
			{
				rb.append("01234567890123456789012345678901", 32);
			}

			rb.rewind();
		}

		std::cout << "recv: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

		// 1백만번, 512 바이트 쓰기, 1.78초 정도. 
		// - 최적화: 최초 버퍼  크기를 1024로 함 --> 0.68초 
		// - 실제 네트워크 코드에서는 32K 이상 확보
		// - 아규먼트로 최초 길이 받고 8K로 설정 --> 0.6초
		// - append를 줄이면 0.267 
		// - send보다 약간 더 빠르다. 
	}

	SECTION("iteartor test")
	{
		resize_buffer rb; 

		REQUIRE(rb.begin() == rb.end());
		REQUIRE(rb.cbegin() == rb.cend());

		rb.append("abc", 3);

		REQUIRE(rb.size() == 3);

		REQUIRE(*rb.begin() == 'a');
		REQUIRE(rb.begin() != rb.end());

		auto iter = rb.begin(); 

		++iter;

		REQUIRE(*iter == 'b');

		iter = rb.begin(); 

		iter += 2;

		REQUIRE(*iter == 'c');

		iter -= 2;

		REQUIRE(*iter == 'a');
	}
}
