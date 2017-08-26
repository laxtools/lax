#include "stdafx.h"

#include <catch.hpp>

#include <array>
#include <iostream>

TEST_CASE("design summary")
{
	SECTION("add transport to session")
	{
		// transport_proto3
		// - 전송 : 
		//   - serialize 
		//   - buffer
		// - 수신 : 
		//   - arena
		//   - 보안
		//     - 패킷 길이 
		//     - 내용 보안
		//       - 개별 필드 제한 
		//       - type / 값 영역 
	}

	SECTION("serialization library")
	{
		// flatbuffers 
		// - small and fast 
		// - object 타겟 언어에서 직접 사용이 안 됨

		// protobuf 
		// - big
		// - object의 필드 등이 개별 메모리로 생성됨 (arena 관련)

		// msgpack 
		// - 가볍고 개별 구현이 있지만 통합 schema에서 코드 생성이 안 됨
	}

	SECTION("정리")
	{
		// msgpack + schema 합친 구조가 이상적
		// - schema --> 코드 (오브젝트 + serialize) 
		// - 생성된 오브젝트 전체를 직접 해당 언어에서 사용

		// https://github.com/pascaldekloe/colfer
		// - 위와 같은 시도 중 하나
		// - 자체 포맷
	}
}

TEST_CASE("msgpack generator design")
{
	SECTION("serialization module")
	{
		// 1) c++로 정의하고 c# 코드를 생성하는 방식. 
		//    - visual c++의 경우 컴파일러 정보 접근이 안 됨 
		//    - clang 같은 걸 사용하는 방식은 가능할 듯. 

		// 2) 외부 파일로 정의하고 c#, c++ 코드를 생성하는 방식. 
		//    - thrift, protobuf 에서 사용하는 방식. 
		//    - 코드가 타겟 언어에 맞게 깔끔해야 함
		//    - protobuf의 java --> wire가 추가됨 (코드 생성 품질을 위해)
	}

	SECTION("관계")
	{
		// session 
		// - set transport for protocol handling
		// - provide diverse transport 

		// serialization is used by transport
		// - on_recv
		// - on_send
	}
}

