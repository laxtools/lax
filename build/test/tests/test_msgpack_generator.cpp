#include "stdafx.h"

#include <catch.hpp>

TEST_CASE("msgpack generator design")
{
	SECTION("serialization module")
	{
		// 1) c++�� �����ϰ� c# �ڵ带 �����ϴ� ���. 
		//    - visual c++�� ��� �����Ϸ� ���� ������ �� �� 
		//    - clang ���� �� ����ϴ� ����� ������ ��. 

		// 2) �ܺ� ���Ϸ� �����ϰ� c#, c++ �ڵ带 �����ϴ� ���. 
		//    - thrift, protobuf ���� ����ϴ� ���. 
		//    - ���̺귯�� �ڵ尡 ũ�ٴ� ���� ������ ���� ����. 
	}

	SECTION("����")
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

		SECTION("project ����")
		{
			// NMake ������Ʈ ����
			// cmake -G "NMake Makefiles" ^
			// -DCMAKE_BUILD_TYPE = Release ^
			// -Dprotobuf_BUILD_TESTS = OFF ^
			// -DCMAKE_INSTALL_PREFIX = .. / .. / .. / .. / install ^
			// .. / ..

			// Visual Studio 2015 win64 project ����
			// cmake - G "Visual Studio 14 2015 Win64" ^
			//	-DCMAKE_INSTALL_PREFIX = .. / .. / .. / .. / install ^
			//	.. / ..

			// http://stackoverflow.com/questions/28350214/how-to-build-x86-and-or-x64-on-windows-from-command-line-with-cmake
			// visual studio command tool ���ÿ��� x64, win32�� ������ 
		}

		SECTION("install")
		{
			// nmake �� nmake install�� �����ϴٰ� �� 

			// solution ���Ϸδ� ��� �����Ѱ�?
			// - INSTALL ������Ʈ�� rebuild�� �ϸ� ���� ������� ����� 
		}

		SECTION("use or not to use it")
		{
			// pros: 
			//  - made in google 
			//  - �پ��� ������ ���. 
			//  - ������ �ڵ� 
			//  - ��� �� �ִ� �ڵ�

			// cons:
			//  - ũ��. 
			//  - �߸� ���ų� ���׸� ���� ���ɼ���?  
			//  - ���θ� �� �𸥴�. 

			// msgpack�� C#�� reflection���� �̽��� ���� 
			// ���� ���, ���� �÷����� �����Ϸ��� msgpack�̳� thrift�� ���� ������ �ʿ�
			// �ϴ�, protobuf�� ����ϰ� ���� ������ msgpack�� idl�� ���� ����ϵ��� ���� 

			// use it till I can develop better one!
		}

		SECTION("enhance usability")
		{
			// pdb�� ����. 
		}
	}

	SECTION("example")
	{

	}
}