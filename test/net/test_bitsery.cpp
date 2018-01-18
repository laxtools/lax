#include "stdafx.h"
#include <catch.hpp>
#include <bitsery/bitsery.h>

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

	}

	SECTION("session integration")
	{
		SECTION("serialization")
		{

		}

		SECTION("define message")
		{

		}

		SECTION("buffer reuse")
		{

		}
	}


}