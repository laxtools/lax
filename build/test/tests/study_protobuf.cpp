#include "stdafx.h"

#include <catch.hpp>

#include <array>

// �����丮�� ���ܵ�

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
			//    - dota2���� �̹� ����ϴ� ��.
			//  - ������ �ڵ� 
			//  - ��� �� �ִ� �ڵ�

			// cons:
			//  - ũ��. 
			//  - �߸� ���ų� ���׸� ���� ���ɼ���?  
			//  - ���θ� �� �𸥴�. 
			//  - ������ �ڵ尡 �������� ���� ��������. (�ͼ������� ��������?)

			// msgpack�� C#�� reflection���� �̽��� ���� 
			// ���� ���, ���� �÷����� �����Ϸ��� msgpack�̳� thrift�� ���� ������ �ʿ�
			// �ϴ�, protobuf�� ����ϰ� ���� ������ msgpack�� idl�� ���� ����ϵ��� ���� 

			// use it till I can develop better one!
		}

		SECTION("enhance usability")
		{
			// pdb�� ����. 
			// - ���� ũ���� � �����Ϸ��� �ʿ�. 

			// lite ������ ����Ϸ���? 
			// - �׳� �ϸ� link ���� �߻�

			// ���� �ð� üũ�� ���� 
			// - üũ�ؼ� ȣ���ϵ��� �ؾ� ��
			// - ���� �ð��� �ſ� ����� �� ����
			// - ���� ���� ���� protoc�� ȣ���ϵ��� �� 

			// stdafx.h ���� ��� Ȯ�� 
			// - ���򸻿��� �� ����.
		}
	}
}

#include "example.pb.h"

using namespace ::google::protobuf;

TEST_CASE("protobuf example")
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	SECTION("simple")
	{
		tutorial::Person person;

		person.set_name("hello");
		person.set_email("message");

		auto phone = person.add_phones();
		phone->set_number("010-9015-3333");
		phone->set_type(tutorial::Person::MOBILE);

		std::string buf;

		person.PrintDebugString();

		// �����ϸ� C++ �ڵ带 ���� ���� .proto �� ���� 
		// �⺻ ������ �ͼ������� ��. 
	}

	SECTION("pack/unpack with buffer")
	{
		tutorial::Person person;

		person.set_name("hello");
		person.set_email("message");

		auto phone = person.add_phones();
		phone->set_number("010");
		phone->set_type(tutorial::Person::MOBILE);

		std::array<char, 128> buf;

		person.SerializeToArray(buf.data(), 128);

		tutorial::Person p2;

		p2.ParseFromArray(buf.data(), 128);

		REQUIRE(p2.name() == "hello");
		REQUIRE(p2.email() == "message");
		REQUIRE(p2.phones_size() == 1);
		REQUIRE(p2.phones(0).number() == "010");
	}

	SECTION("pack/unpack with stream")
	{
		tutorial::Person person;

		person.set_name("hello");
		person.set_email("message");

		auto phone = person.add_phones();
		phone->set_number("010");
		phone->set_type(tutorial::Person::MOBILE);

		// asio::basic_streambuf�� �ּ��� ���� std::ostream os(&b) ���·� ��� ����
		// - �ش� ������ ������ �ߴ� resize ���
		// - �̰ɷδ� ������� ����. ����� �� �� ����. 
	}

	SECTION("memory arena 1")
	{
		google::protobuf::Arena arena;

		auto person = arena.Create<tutorial::Person>(&arena);

		person->set_name("hello");
		person->set_email("message");

		auto phone = person->add_phones();
		phone->set_number("010");
		phone->set_type(tutorial::Person::MOBILE);
	}

	SECTION("memory arena 2")
	{
		auto arena = std::make_shared<Arena>();
		auto person = Arena::Create<tutorial::Person>(arena.get());

		person->set_name("hello");
		person->set_email("message");

		auto phone = person->add_phones();
		phone->set_number("010");
		phone->set_type(tutorial::Person::MOBILE);

		// Arena�� �޼��� ������ �Ҵ� ������ �ϴ� ������ ���� �ϴ°�?
		// �׷� ������ �������� Ȯ���� �𸣰ڴ�. ��� ������ ������ ��.
		// serialize �ѹ��� �Ǵ� �͵�� ó���� ���� �ְڴ�. 
	}
}
