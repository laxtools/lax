#include "stdafx.h"
#include <catch.hpp>
#include <flatbuffers/flatbuffers.h>
// yas warnings
#pragma warning(disable: 4307)
#pragma warning(disable: 4293)
#include <yas/mem_streams.hpp>
#include <yas/binary_iarchive.hpp>
#include <yas/binary_oarchive.hpp>
#include <yas/std_types.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <lax/util/simple_timer.hpp>
#include <iostream>

#include "monster_generated.h"

namespace
{
struct type {
	type()
		:i(33)
		, d(.33)
		, s("33")
		, v({ 33, 33, 33 })
	{}

	std::uint32_t i;
	double d;
	std::string s;
	std::vector<std::uint32_t> v;

};

// one free-function serializer/deserializer
template<typename Ar>
void serialize(Ar &ar, type &t) {
	ar & YAS_OBJECT_NVP(
		"type"
		, ("i", t.i)
		, ("d", t.d)
		, ("s", t.s)
		, ("v", t.v)
	);
}

enum class MyEnum :uint16_t { V1, V2, V3 };
struct MyStruct {
	uint32_t i;
	MyEnum e;
	std::vector<float> fs;
};

//define how object should be serialized/deserialized
template <typename S>
void serialize(S& s, MyStruct& o) {
	s.value4b(o.i);//fundamental types (ints, floats, enums) of size 4b
	s.value2b(o.e);
	s.container4b(o.fs, 10);//resizable containers also requires maxSize, to make it safe from buffer-overflow attacks
}


} // noname

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
			constexpr int test_count = 1;

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

		// endian 처리 지원하고 코드가 깔끔하며 C++ 코드 내에서 정의하는 방식이다. 
		// cereal과 같이 NVP 매크로로 타잎 추론하여 serialization을 한다. 
		// bitsery와 같은 프로젝트도 있으나 이 쪽 구현이 더 나은 것 같다. 
		// 

		SECTION("basic usage")
		{
			type t1;
			yas::mem_ostream os;
			yas::binary_oarchive<yas::mem_ostream> oa(os);
			oa & t1;

			type t2;
			t2.i = 0;
			t2.d = 0;
			t2.s.clear();
			t2.v.clear();

			yas::mem_istream is(os.get_intrusive_buffer());
			yas::binary_iarchive<yas::mem_istream> ia(is);
			ia & t2;

			// 위의 코드 빌드 중 에러 남.
		}

		SECTION("performance")
		{
			constexpr int test_count = 1;

			lax::util::simple_timer timer;

			for (int i = 0; i < test_count; ++i)
			{
				type t1;
				yas::mem_ostream os;
				yas::binary_oarchive<yas::mem_ostream> oa(os);
				oa & t1;

				type t2;
				t2.i = 0;
				t2.d = 0;
				t2.s.clear();
				t2.v.clear();

				yas::mem_istream is(os.get_intrusive_buffer());
				yas::binary_iarchive<yas::mem_istream> ia(is);
				ia & t2;
			}

			std::cout << "yas: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

			// 
			// 백만건, 0.524 
			// msgpack보다는 약간 빠르다. 작은 serialization에 대해 백만건 정도 처리 가능하다.  
			// endian, json serialization, hex dump 등을 지원한다. 
			// 사용성 면에서는 msgpack과 거의 동일하다. 
			//

			// 
			// 차이는 msgpack은 다른 언어 구현이 있다는 점이다. 
			// 메모리 할당 / 해제 관련 부하가 msgpack은 대부분이다. 
			// zone 메모리 관리 때문인데 이 부분 개선하면 매우 빨라질 수 있지 않을까? 
			// 외부에서 메모리를 주는 방식으로 하고 
			// 해당 메모리를 풀링이나 네트워크 버퍼 메모리를 쓰게 한다면 
			// 복사도 줄이고 여러가지 장점이 있을 듯 하다. 
			// 

			// 1천만번 했을 때 shared_buffer에서 할당이 대부분을 차지 
			// 결국은 버퍼 메모리 관리가 중요. 
			// bitsery로 본다. 원하는 방향은 비슷하다. 
		}
	}

	SECTION("bitsery test")
	{
		using namespace bitsery;

		SECTION("basic usage")
		{
			//some helper types
			using Buffer = std::vector<uint8_t>;
			using OutputAdapter = OutputBufferAdapter<Buffer>;
			using InputAdapter = InputBufferAdapter<Buffer>;

			//set some random data
			MyStruct data{ 8941, MyEnum::V2,{ 15.0f, -8.5f, 0.045f } };
			MyStruct res{};

			//create buffer to store data
			Buffer buffer;
			//use quick serialization function,
			//it will use default configuration to setup all the nesessary steps
			//and serialize data to container
			auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

			//same as serialization, but returns deserialization state as a pair
			//first = error code, second = is buffer was successfully read from begin to the end.
			auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);
		}

		SECTION("performance")
		{
			//some helper types
			using Buffer = std::vector<uint8_t>;
			using OutputAdapter = OutputBufferAdapter<Buffer>;
			using InputAdapter = InputBufferAdapter<Buffer>;

			constexpr int test_count = 1000000;

			lax::util::simple_timer timer;

			for (int i = 0; i < test_count; ++i)
			{
				//set some random data
				MyStruct data{ 8941, MyEnum::V2,{ 15.0f, -8.5f, 0.045f } };
				MyStruct res{};

				//create buffer to store data
				Buffer buffer;
				//use quick serialization function,
				//it will use default configuration to setup all the nesessary steps
				//and serialize data to container
				auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

				//same as serialization, but returns deserialization state as a pair
				//first = error code, second = is buffer was successfully read from begin to the end.
				auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);
			}

			std::cout << "yas: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

			//
			// 천만번, 2.898초. 가장 빠르다. Buffer도 제공 가능하다. 버퍼에서 읽을 수도 있다. 
			// 네트워크 처리에 가장 적합해 보인다. 초당 3백만개 정도 처리 가능하다. (작은 패킷들)
			// 빠른 C++ 서버 / 클라 용 엔진인 lax에 가장 적합한 것으로 보인다. 
			//
		}
	}
}
