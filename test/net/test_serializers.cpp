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
	// msgpack�� �������� ������.
	// thrift binary ���� ������. 
	// flatbuffers, yas, capnproto �� �� ������ �ִ� 
	// �ʴ� �鸸 �� �̻��� �����ؾ� �ϴµ� ���� �ʴ�. 
	// �ϴ� flatbuffers�� decode���� 2�鸸 �̻�, ���ڵ��� 30�� �̻��� �����Ѵ�. 
	// ���� ���� type-safe, ��Ƽ�÷���, ��Ƽ��� ���� ������ �� ����. 
	// ������ MSGPACK_DEFINE()ó�� ������ �ʴ�. 
	// �ٽ� �ѹ� �����Ѵ�.
	


	SECTION("capnproto")
	{
		// �� �� ���Ѻ��� �Ѵ�.
		// �ϴ�, �ڵ尡 �����ϰ� ���� ����. 
		// �÷��� ������ ���������� �˱� ��ƴ�. 
		// flatbuffer�� ������ (8K+ likes)�� ���� �ִ�. 
		// msgpack-c ������ ���� �̽��� �ִ�. �ٲ� �� ������??
	}

	SECTION("flatbuffers")
	{
		using namespace flatbuffers;

		// https://code.facebook.com/posts/872547912839369/improving-facebook-s-performance-on-android-with-flatbuffers/
		
		// offset�� ���� vtable�� ���� �����͸� ã�Ƽ� ����. 
		// �޸� �Ľ��� ���� ���ϴ� �����ͷ� ��ȯ��. 
		// �޸� �Ҵ�� �Ľ��� �߰��� ���� ����
		// C++�� flatbuffers.h �ϳ��� ��. ����. 

		// GetMonster() �Լ��� ������ �Ҵ����� �ʰ� access �� ������ 
		// get_hp()�� �ϸ� hp�� ����Ű�� offset���� ���� ���� 
		// ���� �޼����� �޸� ���� �̵��ϰ� �� 

		SECTION("basic usage")
		{
			FlatBufferBuilder fbb;

			fb::Vec3 pos(1, 100, 10);

			// offset ����� �ϸ鼭 ����. ���� ������Ʈ ��ŷ�� ����. Finish�� ����� �� 
			// ������ �� �� ��Ȯ�ϰ� �˷��� ���õ��� �� �ʿ�. 

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

				// offset ����� �ϸ鼭 ����. ���� ������Ʈ ��ŷ�� ����. Finish�� ����� �� 
				// ������ �� �� ��Ȯ�ϰ� �˷��� ���õ��� �� �ʿ�. 

				auto offset = fb::CreateMonsterDirect(fbb, &pos, 300, 600, "hello monster");
				fbb.Finish(offset);

				auto* monster = fb::GetMonster(fbb.GetBufferPointer());

				//REQUIRE(monster->mana() == 300);
				//REQUIRE(monster->hp() == 600);
			}

			std::cout << "flatbuffer: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

			//
			// 0.357 ��.... ���� ������Ʈ�� �۱� �ϴٸ�.
			// ����� �������� 2�� ���� ���̰� ����. 
			// ������ ���� ������ �ϰ� �߰� �޸� ���簡 �Ͼ�� ������ �ӵ��� ���� ��. 
			// 
			// ���� �� �����ϴ� ���� ������ ��� �������� ������ ���� ���̴�. 
			// 
			//
		}
	}

	SECTION("yas")
	{
		// ������ �����ϰ� C++�� �����ؼ� ��ü �޼����� ó�� 
		// C++ Ŭ������ �޼��� ���� Ŭ������ ������ �ϰ� �������� �ʵ��� �Ѵ�. 
		// cereal�� �����ϳ� �� �۰� ������. �ֳ��ϸ� ������ ���� ���� �ۼ� ��İ� ���. 
		// �Լ��� ������ �����ϴ� ����� msgpack�� ����. ���� ���� ������ �� ���� ��. 

		// endian ó�� �����ϰ� �ڵ尡 ����ϸ� C++ �ڵ� ������ �����ϴ� ����̴�. 
		// cereal�� ���� NVP ��ũ�η� Ÿ�� �߷��Ͽ� serialization�� �Ѵ�. 
		// bitsery�� ���� ������Ʈ�� ������ �� �� ������ �� ���� �� ����. 
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

			// ���� �ڵ� ���� �� ���� ��.
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
			// �鸸��, 0.524 
			// msgpack���ٴ� �ణ ������. ���� serialization�� ���� �鸸�� ���� ó�� �����ϴ�.  
			// endian, json serialization, hex dump ���� �����Ѵ�. 
			// ��뼺 �鿡���� msgpack�� ���� �����ϴ�. 
			//

			// 
			// ���̴� msgpack�� �ٸ� ��� ������ �ִٴ� ���̴�. 
			// �޸� �Ҵ� / ���� ���� ���ϰ� msgpack�� ��κ��̴�. 
			// zone �޸� ���� �����ε� �� �κ� �����ϸ� �ſ� ������ �� ���� ������? 
			// �ܺο��� �޸𸮸� �ִ� ������� �ϰ� 
			// �ش� �޸𸮸� Ǯ���̳� ��Ʈ��ũ ���� �޸𸮸� ���� �Ѵٸ� 
			// ���絵 ���̰� �������� ������ ���� �� �ϴ�. 
			// 

			// 1õ���� ���� �� shared_buffer���� �Ҵ��� ��κ��� ���� 
			// �ᱹ�� ���� �޸� ������ �߿�. 
			// bitsery�� ����. ���ϴ� ������ ����ϴ�. 
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
			// õ����, 2.898��. ���� ������. Buffer�� ���� �����ϴ�. ���ۿ��� ���� ���� �ִ�. 
			// ��Ʈ��ũ ó���� ���� ������ ���δ�. �ʴ� 3�鸸�� ���� ó�� �����ϴ�. (���� ��Ŷ��)
			// ���� C++ ���� / Ŭ�� �� ������ lax�� ���� ������ ������ ���δ�. 
			//
		}
	}
}
