#include "stdafx.h"

#include <catch.hpp>

TEST_CASE("msgpack generator design")
{
	SECTION("serialization module")
	{
		// 1) c++로 정의하고 c# 코드를 생성하는 방식. 
		//    - visual c++의 경우 컴파일러 정보 접근이 안 됨 
		//    - clang 같은 걸 사용하는 방식은 가능할 듯. 

		// 2) 외부 파일로 정의하고 c#, c++ 코드를 생성하는 방식. 
		//    - thrift, protobuf 에서 사용하는 방식. 
		//    - 라이브러리 코드가 크다는 점이 마음에 차지 않음. 
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

TEST_CASE("protobuf research")
{
	SECTION("setup and decide")
	{
		// download protoc 
		// download c++ library
		// cmake / build 

		SECTION("project 생성")
		{
			// NMake 프로젝트 생성
			// cmake -G "NMake Makefiles" ^
			// -DCMAKE_BUILD_TYPE = Release ^
			// -Dprotobuf_BUILD_TESTS = OFF ^
			// -DCMAKE_INSTALL_PREFIX = .. / .. / .. / .. / install ^
			// .. / ..

			// Visual Studio 2015 win64 project 생성
			// cmake - G "Visual Studio 14 2015 Win64" ^
			//	-DCMAKE_INSTALL_PREFIX = .. / .. / .. / .. / install ^
			//	.. / ..

			// http://stackoverflow.com/questions/28350214/how-to-build-x86-and-or-x64-on-windows-from-command-line-with-cmake
			// visual studio command tool 선택에서 x64, win32가 결정됨 
		}

		SECTION("install")
		{
			// nmake 는 nmake install로 가능하다고 함 

			// solution 파일로는 어떻게 가능한가?
			// - INSTALL 프로젝트만 rebuild를 하면 빌드 명령으로 실행됨 
		}

		SECTION("use or not to use it")
		{
			// pros: 
			//  - made in google 
			//  - 다양한 곳에서 사용. 
			//  - 오래된 코드 
			//  - 배울 수 있는 코드

			// cons:
			//  - 크다. 
			//  - 잘못 쓰거나 버그를 만날 가능성은?  
			//  - 내부를 잘 모른다. 

			// msgpack은 C#의 reflection에서 이슈가 있음 
			// 여러 언어, 여러 플래폼을 지원하려면 msgpack이나 thrift와 같은 접근이 필요
			// 일단, protobuf를 사용하고 보다 간결한 msgpack을 idl을 통해 사용하도록 진행 

			// use it till I can develop better one!
		}

		SECTION("enhance usability")
		{
			// pdb가 없음. 
		}
	}

	SECTION("example")
	{

	}
}