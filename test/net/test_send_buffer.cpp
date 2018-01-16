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

		// 1백만번, 512 바이트 쓰기, 1초 정도
		// - 최적화 : segs를 reserve로 미리 확보 --> 0.68로 줄어듦
		// - send buffer의 생성이 느림
		//   - 소켓 생성 이후라 실제 append와 transfer는 빠를 듯
		//   - 아님. append()의 호출에 의존
		//   - 얼마나 자주 append() (결국 memcpy)가 호출 되는 지에 따름
		//   - 크기보다 횟수에 의존. 0.411까지 줄어듦. 0.411일 경우 1.1GB 정도.
		//   - 16번으로 줄이면 1.7GB까지 상승함. 
		//   - serialize 부하가 적을 경우 아주 빠를 듯

		// check 매크로를 릴리스 모드에서 켜면 꽤 느려진다. 100ms 정도 (백만번 기존) 

		// I7, 캐시 버그 패치 안 한 장비에서 0.282초
		// I5, 캐시 버그 패치 한 장비에서 0.379초
	}
}

