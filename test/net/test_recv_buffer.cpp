#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/detail/buffer/recv_buffer.hpp>
#include <lax/util/simple_timer.hpp>
#include <iostream>

using namespace lax::net;

TEST_CASE("recv buffer")
{
	SECTION("basic usage")
	{
		recv_buffer rb; 

		REQUIRE(rb.append("abc", 3) == 3);
		REQUIRE(rb.data()[0] == 'a');
	}

	SECTION("resizing")
	{
		recv_buffer rb(1024); 

		for (int i = 0; i < 1024; ++i)
		{
			rb.append("a", 1);
		}

		REQUIRE(rb.capacity() == 1024);
		REQUIRE(rb.get_pool(1024)->get_stat().alloc_count == 1);
	}

	SECTION("pop front")
	{
		recv_buffer rb; 

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
			recv_buffer rb(1024*8);

			for (int k = 0; k < 16; ++k)
			{
				rb.append("01234567890123456789012345678901", 32);
			}

			rb.rewind();
		}

		std::cout << "recv: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

		// 1�鸸��, 512 ����Ʈ ����, 1.78�� ����. 
		// - ����ȭ: ���� ����  ũ�⸦ 1024�� �� --> 0.68�� 
		// - ���� ��Ʈ��ũ �ڵ忡���� 32K �̻� Ȯ��
		// - �ƱԸ�Ʈ�� ���� ���� �ް� 8K�� ���� --> 0.6��
		// - append�� ���̸� 0.267 
		// - send���� �ణ �� ������. 
	}
}
