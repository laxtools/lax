#include "stdafx.h"

#include <catch.hpp>

#include <array>

// 히스토리로 남겨둠

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
			//    - dota2에서 이미 사용하는 듯.
			//  - 오래된 코드 
			//  - 배울 수 있는 코드

			// cons:
			//  - 크다. 
			//  - 잘못 쓰거나 버그를 만날 가능성은?  
			//  - 내부를 잘 모른다. 
			//  - 생성된 코드가 가독성이 많이 떨어진다. (익숙해지면 괜찮을까?)

			// msgpack은 C#의 reflection에서 이슈가 있음 
			// 여러 언어, 여러 플래폼을 지원하려면 msgpack이나 thrift와 같은 접근이 필요
			// 일단, protobuf를 사용하고 보다 간결한 msgpack을 idl을 통해 사용하도록 진행 

			// use it till I can develop better one!
		}

		SECTION("enhance usability")
		{
			// pdb가 없음. 
			// - 서버 크래시 등에 대응하려면 필요. 

			// lite 버전을 사용하려면? 
			// - 그냥 하면 link 에러 발생

			// 파일 시간 체크가 없음 
			// - 체크해서 호출하도록 해야 함
			// - 빌드 시간이 매우 길어질 수 있음
			// - 별도 툴을 만들어서 protoc를 호출하도록 함 

			// stdafx.h 포함 기능 확인 
			// - 도움말에는 잘 없다.
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

		// 정리하면 C++ 코드를 보지 말고 .proto 만 보고 
		// 기본 사용법에 익숙해지면 끝. 
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

		// asio::basic_streambuf의 주석을 보면 std::ostream os(&b) 형태로 사용 가능
		// - 해당 구현은 이전에 했던 resize 방식
		// - 이걸로는 충분하지 않음. 고려할 게 더 많다. 
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

		// Arena가 메세지 단위로 할당 해제를 하는 것으로 봐야 하는가?
		// 그런 것으로 보이지만 확실히 모르겠다. 어느 정도가 적당한 지.
		// serialize 한번에 되는 것들로 처리할 수도 있겠다. 
	}
}
