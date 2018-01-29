#include "stdafx.h"
#include <catch.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/ext/inheritance.h>
#include <lax/net/detail/buffer/resize_buffer.hpp>
#include <lax/util/simple_timer.hpp>
#include <iostream>

using namespace bitsery;
using namespace lax::net;

using bitsery::ext::BaseClass;
using bitsery::ext::VirtualBaseClass;

// �����ϸ鼭 ���踦 �����ϱ� ���� �׽�Ʈ�� �����丮�� ����� �׽�Ʈ. 
// - bits_message�� �������̽� ã�� 
// - �޼����� struct ������ �����ϱ� 
// - resize_buffer�� serialization �����ϰ� ����� 
// - ���� ó�� ��� ã�� 
// - ���� �����ϰ� �Ϻ� ���� 
// 
// ���� �۾��� �Ϸ��ߴ�. 

namespace
{

enum class MyEnum : uint16_t
{
	V1, V2, V3
};

struct MyStruct {
	uint32_t i;
	MyEnum e;
	std::string name;
	std::vector<float> fs;
};

struct DerivedStruct : public MyStruct
{
	int32_t v;
};

struct EmbedStruct
{
	MyStruct my;
	int32_t  v;
};

//define how object should be serialized/deserialized
template <typename S>
void serialize(S& s, MyStruct& o)
{
	s.value4b(o.i);			// fundamental types (ints, floats, enums) of size 4b
	s.value2b(o.e);
	s.text1b(o.name, 256);
	s.container4b(o.fs, 10); //resizable containers also requires maxSize, to make it safe from buffer-overflow attacks
}

template <typename S>
void serialize(S& s, DerivedStruct& o)
{
	s.ext(o, BaseClass<MyStruct>{});
	s.value4b(o.v);
}

template <typename S>
void serialize(S& s, EmbedStruct& o) 
{
	//	serialize(s, o.my); // guess work. �Ʒ��� object�� �ϰ����� ���� ���. ���� �������� ���
	s.object(o.my);
	s.value4b(o.v);
}

} // noname


namespace bitsery
{
namespace traits
{

template<>
struct ContainerTraits<lax::net::resize_buffer>
	:public StdContainer<lax::net::resize_buffer, true, true> 
{
};

template<>
struct BufferAdapterTraits<lax::net::resize_buffer> 
	: public StdContainerForBufferAdapter<lax::net::resize_buffer, true> 
{
};



//
// ContainerTraits�� iterator�� �ʿ�� �Ѵ�. 
// array iterator�� ũ�Ⱑ ������ ���̶� ���Ⱑ �������� �ʴ�. 
// 
// Contiguous Iterator, RandomAccessIterator, Iterator 
// - concepts in C++, which is the power of C++ 
// 
// �̷� �͵��� ���� �̹� ���� ������� ����� ��￴�� 
// C++ 11 ������ �� �ۼ��� ����. 

// [1] std::iterator���� tag�� �Բ� ��� �ް� 
// �ʼ������� �����ؾ� �� �Լ��� tag�� ����Ͽ� �ۼ��Ѵ�. 
//

// [2] �ش� Ŭ�������� begin / end, cbegin / cend �� �ۼ��Ѵ�. 
// iterator�� �����ؼ� �����ش�. 


} // traits
} // bitsery

//some helper types
using Buffer = std::vector<uint8_t>;
using OutputAdapter = OutputBufferAdapter<Buffer>;
using InputAdapter = InputBufferAdapter<Buffer>;

TEST_CASE("bitsery test")
{
	//
	// test_serializers �׽�Ʈ�� ���� bitsery�� msgpack�� ���� 3�� ����, 
	// flatbuffers�� ���ؼ� 2�� ���� ���� ������ Ȯ�� �Ǿ���. 
	// yas�� �����ƺ������� shared_buffer�� ���ο� �־� 
	// ��Ʈ��ũ ��� �� ���縦 ���� �� ����. bitsery�� �̸� ����ؼ� ����Ǿ��� 
	// ���۸� ������ �� �ִ� �����̴�. 
	// bitsery�� ���ۿ� ���� �ӵ��� ������ �ӵ��� �����ش�.
	// 
	// �ᱹ serialization ������ �޸� �Ҵ�/������ ���縦 �󸶳� ���� �� �ִ� ���� �߿��ϴ�. 
	// msgpack�� ��� parsing ���ϵ� �� �Ǵ� ���̴�. 
	// C++ Ŭ�� / ������ ���� ������ ������ ó���� �����ϴ� �� lax�� ��ǥ�̹Ƿ� 
	// �̸� ������ ������ ���̺귯���� ���δ�. 
	// flatbuffers�� ����� �������� ���� �����ϰ� 
	// �پ��� �� ������ ���� ���������� lax���� ���� �ʴ�. 
	// 

	SECTION("usage and concepts")
	{
		SECTION("basic types in class")
		{
			MyStruct data{ 8941, MyEnum::V2, "hello", { 15.0f, -8.5f, 0.045f } };
			MyStruct res{};

			Buffer buffer;

			auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

			auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

			REQUIRE(res.name == data.name);
		}

		SECTION("inheritance")
		{
			// use inheritance adpater

			// ������� ó���ϴ� �ͺ��ٴ� �������� ó���ϴ� �� 
			// initializer list �� ����ϱ� ���ϴ�. 

			DerivedStruct data;
			data.v = 3; 
			data.e = MyEnum::V2; 
			data.name = "Hello";

			DerivedStruct res{};

			Buffer buffer;

			auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

			auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

			REQUIRE(res.name == data.name);
		}

		SECTION("embedding")
		{
			EmbedStruct data{ { 8941, MyEnum::V2, "hello", { 15.0f, -8.5f, 0.045f } }, 3 };
			EmbedStruct res{};

			Buffer buffer;

			auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

			auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

			REQUIRE(res.my.name == data.my.name);
		}
	}

	SECTION("session integration")
	{
		SECTION("buffer reuse")
		{
			//
			// [1] Buffer�� ���� 
			// buffer.h�� �ִ� Adapter�� �����ؾ� �Ѵ�. 
			// ������ std::array, std::vector�� �ִ�. 
			// �ܺ� ���۸� ����ϴ� ������ ���� ���� �ϴ� ����. 

			// ContainerTraits 
			// BufferAdapterTraits

			// [2] Stream���� ���� 
			// C++ stream�� �Լ����� ����. 
			// ����, �� �� ���ٴ� ���� ���� �� ������ �� 

			// 
			// send_buffer�� ���� OutputAdapter�� �����ϰ� 
			// recv_buffer�� ���� InputAdapter�� �����ؾ� �Ѵ�.
			// �׷��� ���縦 ������ �� �ִ�. 
			// 

			//
			// Buffer�� ����� 
			// - �ſ� ���� C++ ���� ����
			//
		

			using lax::net::resize_buffer;

			SECTION("OutputAdapter")
			{
				using OutputAdapter = OutputBufferAdapter<resize_buffer>;

				MyStruct data{ 8941, MyEnum::V2, "hello",{ 15.0f, -8.5f, 0.045f } };


				resize_buffer buffer;

				auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

				REQUIRE(writtenSize > 0);

				// ���ӵ� �޸𸮸� ������ �ϰ� ���� �����ؾ� �� 
				// resize_buffer

				// ����� �Ǿ��µ� writerInternal���� iterator ������ ���� ���װ� �ִ�. 
				// ��... iterator�� �Ϲ����� ����� �� �ȴ�. 
				// ������ ������ �ٸ���.   
				// resize()�� �ǹ̰� reserve and resize�� �ǹ��̴�. 
				// ���Ϳ� �����ϰ� �����ߴ�. ���� 0���� �ʱ�ȭ. (�ʿ��Ѱ�?) 
			}

			SECTION("InputAdapter")
			{
				using OutputAdapter = OutputBufferAdapter<resize_buffer>;
				using InputAdapter = InputBufferAdapter<resize_buffer>;

				MyStruct data{ 8941, MyEnum::V2, "hello",{ 15.0f, -8.5f, 0.045f } };
				MyStruct res;

				resize_buffer buffer;

				auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);
				auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

				REQUIRE(data.name == res.name);
			}

			SECTION("performance")
			{
				constexpr int test_count = 1000;

				lax::util::simple_timer timer;

				for (int i = 0; i < test_count; ++i)
				{
					using Buf = resize_buffer;
					using OutputAdapter = OutputBufferAdapter<Buf>;
					using InputAdapter = InputBufferAdapter<Buf>;

					MyStruct data{ 8941, MyEnum::V2, "hello",{ 15.0f, -8.5f, 0.045f } };
					MyStruct res;

					Buf buffer;

					auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);
					auto state = quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

					// REQUIRE(data.name == res.name);
				}

				std::cout << "bitsery w/ resize_buffer: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

				//
				// REQUIRE ���� 0.419, vector�� 0.384. ���Ͱ� �� ���� ������? 
				// release�� ����. 0.394 ���� ���󰬴�. 
				// üũ�� ���Ƽ� �� �� ���ȴ�. std::allocator�� ����� ���� �� �� �� �ִ�. 
				// 0.358���� ����. shared_ptr�� ������ �ѱ�� ��������. 
				// concurrent_queue::push�� ���� �ɸ���. ���� ���� ���ٴ� ��������. 
				// �ʴ� 270�� ���� �Ѿ ó�� �����ϴ�. 

				// ���� ��Ʈ��ũ �����ؼ� ���� ���� ������ �ȴٸ� �ſ� ������.
				// �߰� �δ���� ����Ƿ� �� ������ �� �� ��. 
			}

			SECTION("error handling")
			{
				// �߿�. 
				using Buf = resize_buffer;
				using OutputAdapter = OutputBufferAdapter<Buf>;
				using InputAdapter = InputBufferAdapter<Buf>;

				MyStruct data{ 8941, MyEnum::V2, "hello",{ 15.0f, -8.5f, 0.045f } };
				MyStruct res;

				Buf buffer;

				// �� ���� �޸� �Ҵ縸 �Ǹ� ������ �߻��� �� ����. 
				BasicSerializer <AdapterWriter<OutputAdapter, bitsery::DefaultConfig>> ser{ buffer };
				ser.object(data);

				auto& w = AdapterAccess::getWriter(ser);
				w.flush();

				auto writtenSize = w.writtenBytesCount();
				REQUIRE(writtenSize > 0);

				// ���� ���� �߰��� ���� �����ϹǷ� ������ �߻��� �� �ִ�. 
				BasicDeserializer <AdapterReader<InputAdapter, bitsery::DefaultConfig>> des{ InputAdapter{ buffer.begin(), writtenSize } };
				des.object(res);

				auto& r = AdapterAccess::getReader(des);

				REQUIRE(r.error() == ReaderError::NoError);
				REQUIRE(r.isCompletedSuccessfully());

				// ���� �� �κ��� bits_message �� pack / unpack �Լ� ���� ���� 
				// �ű⼭ ���� üũ�� �Ѵ�. 
			}
		}

		SECTION("message interface design")
		{
			//
			// ref::send( msg::ptr m ); 
			// - ���⼭ Ÿ���� �Ұ� ��. 
			// - session::send() 
			// - bits_proto::send( m )
			// - m->serialize( send_buffer )
			// - bits_res_login::pack( buf )
			// -   quickserialize<OutputAdapter>( buf, *this );
			//    
			// 
			// �� ������ �����ؾ� �� 
			// BITS_MSG() 
			//  - pack(), unpack() 
			// 
			// serialize( S& s, bits_res_login& o)
			// - �� �Լ��� serialize() �� namespace ���� ���� �����ؾ� ��
			// 

			//
			// ���� Ŭ������ ����� Ȯ��
			// �� ���� msgpack���� ������ C++ ���� ��İ� �� �����ϴ�. 
			// 

			// ���� ó��: 
			// quickSerialize���� ������ ��� ���� ó�� ����� ã�´�. 
			// - Adaptor�� ������ �����ϴ� �ڵ�� �ִµ� move�� �ѱ�� ������ ã�� �� ���� �� �ѵ�...
			
		}

		SECTION("consider performance")
		{
			//
			// send copy : 2, lock : copy bytes from protocol to session
			// - protocol::send(m) 
			//   - serialized to a temp buffer from a pool. no lock
			//     - resize_buffer�� ����ϸ� Ǯ�� �ڵ����� �����
			//   - copied to session's send buffer (lock'ed)
			// 
			
			// 
			// recv copy: 1, no lock 
			// - session::recv
			//   - copied to protocol recv buffer 
			//     - if recv buffer pointer is used directly, then we don't need to copy. 
			//     - however, buffer management becomes complicated. 1 copy seems reasonable.
			//   - read type and create a msg
			//   - deserialized from recv buffer (no lock)
			//

			// to achieve above, 
		}

		SECTION("endian setup")
		{
			// default�� little endian ����. �ƴϸ� endian Ȯ��?
			// 
		}
	}
}

