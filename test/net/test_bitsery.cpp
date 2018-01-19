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

// 이해하면서 설계를 진행하기 위한 테스트로 히스토리에 가까운 테스트. 
// - bits_message의 인터페이스 찾기 
// - 메세지를 struct 등으로 구성하기 
// - resize_buffer로 serialization 가능하게 만들기 
// - 에러 처리 방법 찾기 
// - 성능 측정하고 일부 개선 
// 
// 위의 작업을 완료했다. 

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
	//	serialize(s, o.my); // guess work. 아래의 object를 일관성을 위해 사용. 내부 구현에서 사용
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
// ContainerTraits는 iterator를 필요로 한다. 
// array iterator는 크기가 고정된 것이라 쓰기가 적합하지 않다. 
// 
// Contiguous Iterator, RandomAccessIterator, Iterator 
// - concepts in C++, which is the power of C++ 
// 
// 이런 것들을 위해 이미 여러 사람들이 노력을 기울였고 
// C++ 11 에서는 더 작성이 쉽다. 

// [1] std::iterator에서 tag와 함께 상속 받고 
// 필수적으로 구현해야 할 함수를 tag를 고려하여 작성한다. 
//

// [2] 해당 클래스에서 begin / end, cbegin / cend 를 작성한다. 
// iterator를 생성해서 돌려준다. 


} // traits
} // bitsery

//some helper types
using Buffer = std::vector<uint8_t>;
using OutputAdapter = OutputBufferAdapter<Buffer>;
using InputAdapter = InputBufferAdapter<Buffer>;

TEST_CASE("bitsery test")
{
	//
	// test_serializers 테스트를 통해 bitsery가 msgpack에 비해 3배 정도, 
	// flatbuffers에 비해서 2배 정도 빠른 것으로 확인 되었다. 
	// yas도 괜찮아보이지만 shared_buffer가 내부에 있어 
	// 네트워크 통신 시 복사를 피할 수 없다. bitsery는 이를 고려해서 설계되었고 
	// 버퍼를 제공할 수 있는 구조이다. 
	// bitsery는 버퍼에 쓰는 속도에 근접한 속도를 보여준다.
	// 
	// 결국 serialization 성능은 메모리 할당/해제와 복사를 얼마나 줄일 수 있는 지가 중요하다. 
	// msgpack의 경우 parsing 부하도 꽤 되는 편이다. 
	// C++ 클라 / 서버만 가장 빠르고 안정된 처리를 지원하는 게 lax의 목표이므로 
	// 이를 구현할 최적의 라이브러리로 보인다. 
	// flatbuffers는 사용이 생각보다 많이 불편하고 
	// 다양한 언어를 지원할 때는 유용하지만 lax에는 맞지 않다. 
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

			// 상속으로 처리하는 것보다는 포함으로 처리하는 게 
			// initializer list 를 사용하기 편하다. 

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
			// [1] Buffer로 동작 
			// buffer.h에 있는 Adapter를 구현해야 한다. 
			// 예제는 std::array, std::vector만 있다. 
			// 외부 버퍼를 사용하는 구조에 대한 얘기는 일단 없다. 

			// ContainerTraits 
			// BufferAdapterTraits

			// [2] Stream으로 동작 
			// C++ stream은 함수들이 많다. 
			// 따라서, 이 쪽 보다는 버퍼 쪽이 더 간단할 듯 

			// 
			// send_buffer를 통해 OutputAdapter가 동작하고 
			// recv_buffer를 통해 InputAdapter가 동작해야 한다.
			// 그래야 복사를 제거할 수 있다. 
			// 

			//
			// Buffer로 만들기 
			// - 매우 좋은 C++ 연습 문제
			//
		

			using lax::net::resize_buffer;

			SECTION("OutputAdapter")
			{
				using OutputAdapter = OutputBufferAdapter<resize_buffer>;

				MyStruct data{ 8941, MyEnum::V2, "hello",{ 15.0f, -8.5f, 0.045f } };


				resize_buffer buffer;

				auto writtenSize = quickSerialization<OutputAdapter>(buffer, data);

				REQUIRE(writtenSize > 0);

				// 연속된 메모리를 가져야 하고 증가 가능해야 함 
				// resize_buffer

				// 빌드는 되었는데 writerInternal에서 iterator 관련한 구현 버그가 있다. 
				// 흠... iterator의 일반적인 사용은 잘 된다. 
				// 최초의 동작이 다르다.   
				// resize()의 의미가 reserve and resize의 의미이다. 
				// 벡터와 동일하게 정리했다. 값을 0으로 초기화. (필요한가?) 
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
				// REQUIRE 빼면 0.419, vector는 0.384. 벡터가 더 빠른 이유는? 
				// release가 느림. 0.394 까지 따라갔다. 
				// 체크가 많아서 좀 더 느렸다. std::allocator도 상당히 빠른 걸 알 수 있다. 
				// 0.358까지 진행. shared_ptr도 참조로 넘기면 빨라진다. 
				// concurrent_queue::push가 조금 걸린다. 이제 벡터 보다는 빨라졌다. 
				// 초당 270만 개를 넘어서 처리 가능하다. 

				// 실제 네트워크 연결해서 위와 같은 수준이 된다면 매우 빠르다.
				// 추가 부담들이 생기므로 그 정도는 안 될 듯. 
			}

			SECTION("error handling")
			{
				// 중요. 
				using Buf = resize_buffer;
				using OutputAdapter = OutputBufferAdapter<Buf>;
				using InputAdapter = InputBufferAdapter<Buf>;

				MyStruct data{ 8941, MyEnum::V2, "hello",{ 15.0f, -8.5f, 0.045f } };
				MyStruct res;

				Buf buffer;

				// 쓸 때는 메모리 할당만 되면 에러가 발생할 수 없다. 
				BasicSerializer <AdapterWriter<OutputAdapter, bitsery::DefaultConfig>> ser{ buffer };
				ser.object(data);

				auto& w = AdapterAccess::getWriter(ser);
				w.flush();

				auto writtenSize = w.writtenBytesCount();
				REQUIRE(writtenSize > 0);

				// 읽을 때는 중간에 조작 가능하므로 에러가 발생할 수 있다. 
				BasicDeserializer <AdapterReader<InputAdapter, bitsery::DefaultConfig>> des{ InputAdapter{ buffer.begin(), writtenSize } };
				des.object(res);

				auto& r = AdapterAccess::getReader(des);

				REQUIRE(r.error() == ReaderError::NoError);
				REQUIRE(r.isCompletedSuccessfully());

				// 위의 각 부분이 bits_message 의 pack / unpack 함수 내에 들어가고 
				// 거기서 에러 체크를 한다. 
			}
		}

		SECTION("message interface design")
		{
			//
			// ref::send( msg::ptr m ); 
			// - 여기서 타잎을 잃게 됨. 
			// - session::send() 
			// - bits_proto::send( m )
			// - m->serialize( send_buffer )
			// - bits_res_login::pack( buf )
			// -   quickserialize<OutputAdapter>( buf, *this );
			//    
			// 
			// 두 가지를 구현해야 함 
			// BITS_MSG() 
			//  - pack(), unpack() 
			// 
			// serialize( S& s, bits_res_login& o)
			// - 이 함수는 serialize() 로 namespace 없이 접근 가능해야 함
			// 

			//
			// 샘플 클래스를 만들고 확인
			// 이 쪽은 msgpack보다 기존의 C++ 서버 방식과 더 유사하다. 
			// 

			// 에러 처리: 
			// quickSerialize에서 실패할 경우 에러 처리 방법을 찾는다. 
			// - Adaptor에 에러를 지정하는 코드는 있는데 move로 넘기기 때문에 찾을 수 없을 듯 한데...
			
		}

		SECTION("consider performance")
		{
			//
			// send copy : 2, lock : copy bytes from protocol to session
			// - protocol::send(m) 
			//   - serialized to a temp buffer from a pool. no lock
			//     - resize_buffer를 사용하면 풀을 자동으로 사용함
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
			// default가 little endian 고정. 아니면 endian 확인?
			// 
		}
	}
}

