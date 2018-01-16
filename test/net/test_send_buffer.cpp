#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/detail/buffer/send_buffer.hpp>
#include <lax/util/simple_timer.hpp>
#include <iostream>

using namespace lax::net;

TEST_CASE("send buffer")
{
	SECTION("basic case")
	{
		send_buffer<7> sb; 

		REQUIRE(sb.append("abc", 3) == 3);
		REQUIRE(sb.append("cde", 3) == 3);
		REQUIRE(sb.append("fgh", 3) == 3);

		REQUIRE(sb.size() == 9);
		REQUIRE(sb.get_write_seg() == 1);
		REQUIRE(sb.get_data_seg() == 1);

		for (int i = 0; i < 128; ++i)
		{
			REQUIRE(sb.append("aaa", 3) == 3);
			REQUIRE(sb.size() == 9 + (i + 1) * 3);
		}
	}

	SECTION("edge cases")
	{
		send_buffer<7> sb; 

		REQUIRE(sb.append("abcdefg", 7) == 7);
		REQUIRE(sb.get_write_seg() == 1);
		REQUIRE(sb.get_data_seg() == 0);

		auto lst = sb.transfer();

		REQUIRE(lst.size() == 1);
		REQUIRE(sb.size() == 0);
		REQUIRE(sb.get_write_seg() == 0);
		REQUIRE(sb.get_data_seg() == 0);

		sb.release(lst);
	}

	SECTION("simple performance")
	{
		constexpr int test_count = 1000000;

		std::vector<send_buffer<1024 * 8>> sbs;

		for (int i = 0; i < test_count; ++i)
		{
			sbs.emplace_back(send_buffer<1024 * 8>());
		}

		lax::util::simple_timer timer;

		for (int i = 0; i < test_count; ++i)
		{
			for (int k = 0; k < 16; ++k)
			{
				sbs[i].append("01234567890123456789012345678901", 32);
			}

			auto segs = sbs[i].transfer();

			sbs[i].release(segs);
		}

		std::cout << "send: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

		// 1�鸸��, 512 ����Ʈ ����, 1�� ����
		// - ����ȭ : segs�� reserve�� �̸� Ȯ�� --> 0.68�� �پ��
		// - send buffer�� ������ ����
		//   - ���� ���� ���Ķ� ���� append�� transfer�� ���� ��
		//   - �ƴ�. append()�� ȣ�⿡ ����
		//   - �󸶳� ���� append() (�ᱹ memcpy)�� ȣ�� �Ǵ� ���� ����
		//   - ũ�⺸�� Ƚ���� ����. 0.411���� �پ��. 0.411�� ��� 1.1GB ����.
		//   - 16������ ���̸� 1.7GB���� �����. 
		//   - serialize ���ϰ� ���� ��� ���� ���� ��

		// check ��ũ�θ� ������ ��忡�� �Ѹ� �� ��������. 100ms ���� (�鸸�� ����) 

		// I7, ĳ�� ���� ��ġ �� �� ��񿡼� 0.282��
		// I5, ĳ�� ���� ��ġ �� ��񿡼� 0.379��
	}
}

