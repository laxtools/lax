#include "stdafx.h"
#include <catch.hpp>
#include <flatbuffers/flatbuffers.h>
#include <lax/util/simple_timer.hpp>
#include <iostream>

#include "monster_generated.h"

TEST_CASE("serializers")
{
	//
	// msgpack이 생각보다 느리다.
	// thrift binary 보다 느리다. 
	// flatbuffers, yas, capnproto 등 몇 가지가 있다 
	// 초당 백만 개 이상을 지원해야 하는데 쉽지 않다. 
	// 일단 flatbuffers가 decode에서 2백만 이상, 인코딩에 30만 이상을 지원한다. 
	// 가장 빠른 type-safe, 멀티플래폼, 멀티언어 지원 포맷인 것 같다. 
	// 사용법이 MSGPACK_DEFINE()처럼 쉽지는 않다. 
	// 다시 한번 정리한다.
	


	SECTION("capnproto")
	{
		// 좀 더 지켜봐야 한다.
		// 일단, 코드가 복잡하고 양이 많다. 
		// 플래폼 지원이 안정적인지 알기 어렵다. 
		// flatbuffer가 인지도 (8K+ likes)를 갖고 있다. 
		// msgpack-c 구현이 성능 이슈가 있다. 바꿀 수 있을까??
	}

	SECTION("flatbuffers")
	{
		using namespace flatbuffers;

		// https://code.facebook.com/posts/872547912839369/improving-facebook-s-performance-on-android-with-flatbuffers/
		
		// offset을 갖는 vtable을 갖고 포인터를 찾아서 읽음. 
		// 메모리 파싱은 없고 원하는 데이터로 변환됨. 
		// 메모리 할당과 파싱이 중간에 없어 빠름
		// C++은 flatbuffers.h 하나로 됨. 멋짐. 

		// GetMonster() 함수는 뭔가를 할당하지 않고 access 만 가능함 
		// get_hp()를 하면 hp를 가리키는 offset에서 값을 읽음 
		// 따라서 메세지와 메모리 블럭이 이동하게 됨 

		SECTION("basic usage")
		{
			FlatBufferBuilder fbb;

			fb::Vec3 pos(1, 100, 10);

			// offset 계산을 하면서 진행. 여러 오브젝트 패킹이 가능. Finish를 해줘야 됨 
			// 사용법을 좀 더 정확하게 알려면 샘플들이 더 필요. 

			auto offset = fb::CreateMonsterDirect(fbb, &pos, 300, 600, "hello monster");
			fbb.Finish(offset);

			auto* monster = fb::GetMonster(fbb.GetBufferPointer());

			REQUIRE(monster->mana() == 300);
			REQUIRE(monster->hp() == 600);
		}

		SECTION("basic performance")
		{
			constexpr int test_count = 1000000;

			lax::util::simple_timer timer;

			for (int i = 0; i < test_count; ++i)
			{
				FlatBufferBuilder fbb;

				fb::Vec3 pos(1, 100, 10);

				// offset 계산을 하면서 진행. 여러 오브젝트 패킹이 가능. Finish를 해줘야 됨 
				// 사용법을 좀 더 정확하게 알려면 샘플들이 더 필요. 

				auto offset = fb::CreateMonsterDirect(fbb, &pos, 300, 600, "hello monster");
				fbb.Finish(offset);

				auto* monster = fb::GetMonster(fbb.GetBufferPointer());

				//REQUIRE(monster->mana() == 300);
				//REQUIRE(monster->hp() == 600);
			}

			std::cout << "flatbuffer: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

			//
			// 0.357 초.... 물론 오브젝트가 작긴 하다만.
			// 비슷한 구성에서 2배 정도 차이가 난다. 
			// 사용법을 쉽게 만들어야 하고 추가 메모리 복사가 일어나면 동일한 속도를 보일 듯. 
			// 
			// 여러 언어를 지원하는 점도 마음에 들고 지속적인 개선은 있을 것이다. 
			// 
			//
		}
	}

	SECTION("yas")
	{
		// 빠르고 간결하고 C++로 구성해서 전체 메세지를 처리 
		// C++ 클래스와 메세지 구성 클래스가 갖도록 하고 복잡하지 않도록 한다. 
		// cereal과 유사하나 더 작고 빠르다. 왜냐하면 기존의 게임 서버 작성 방식과 비슷. 
		// 함수를 일일이 지정하는 방식은 msgpack과 같다. 실제 내부 구현이 더 빠른 듯. 

		// 
	}
}
