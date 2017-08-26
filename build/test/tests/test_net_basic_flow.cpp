#include "stdafx.h"

#include <lax/net/service.h>
#include <lax/util/logger.h>
#include "test_app.h"

#include <catch.hpp>
#include <conio.h>
#include <memory>
#include <iostream>
#include <map>
#include <thread>

using namespace lax::net;

namespace
{

/// string protocol session to test overall flow
class session_text_echo : public session
{
public:
	session_text_echo(
		service& svc,
		const id& id,
		tcp::socket&& soc,
		bool accepted)
		: session(svc, id, std::move(soc), accepted)
	{
		send_echo();
	}

	~session_text_echo()
	{
	}

	std::atomic<int> send_count = 0;
	std::atomic<int> recv_count = 0;

private:
	void on_created() override
	{

	}

	result on_recv(const uint8_t* data, std::size_t len) override
	{
		++recv_count; 

		const char* msg = (const char*)data;

		// lax::util::log_helper::get()->info( msg );

		send_echo();

		return result(true, reason::success);
	}

	result on_send(uint8_t* data, std::size_t len)  override
	{
		return result(true, reason::success);
	}

	void on_error(const asio::error_code& ec) override
	{
		// nothing to do. 
		// 
	}

	void send_echo()
	{
		std::string msg = "hello session!";

		send((uint8_t*)msg.c_str(), msg.length());

		++send_count;
	}
};

} // noname

TEST_CASE("net simple echo")
{
	lax::test::test_app ap;

	service svc(&ap);

	service::creator func = [] // vs 2015 intellisense 오류
	(
		service& svc,
		const session::id& id,
		tcp::socket&& soc,
		bool accepted
		)
	{
		return session::ptr(
			new session_text_echo(
				svc,
				id,
				std::move(soc),
				accepted));
	};

	REQUIRE(svc.listen("127.0.0.1:7777", func));
	REQUIRE(svc.connect("127.0.0.1:7777", func));

	// give time to process
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	{
		auto p1 = std::static_pointer_cast<session_text_echo>(svc.get(session::id(1, 0)));

		REQUIRE(p1);
		REQUIRE(p1->recv_count > 0);
		REQUIRE(p1->send_count > 0);
	}

	{
		auto p1 = std::static_pointer_cast<session_text_echo>(svc.get(session::id(2, 1)));

		REQUIRE(p1);
		REQUIRE(p1->recv_count > 0);
		REQUIRE(p1->send_count > 0);
	}

	// Bug History: 
	// - creator가 connector와 acceptor에서 empty로 나옴. reference에서 복사로 변경 : ok
	// - on_connected에서 세션 보관 하는 부분 누락 된 것 수정 : ok 

}

#if 1
TEST_CASE("net connection test")
{
	lax::test::test_app ap;

	service svc(&ap);

	service::creator func = [] // vs 2015 intellisense 오류
	(
		service& svc,
		const session::id& id,
		tcp::socket&& soc,
		bool accepted
		)
	{
		return session::ptr(
			new session_text_echo(
				svc,
				id,
				std::move(soc),
				accepted));
	};

	REQUIRE(svc.listen("0.0.0.0:7777", func));

	const int test_count = 10;

	for (int i = 0; i < test_count; ++i)
	{
		REQUIRE(svc.connect("127.0.0.1:7777", func));
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Bug History : 
	// - on_recv_completed 중에 runtime 에러 발생 
	//  test_Debug_x64.exe!issue_debug_notification(const wchar_t * const message) 줄 125	C++
	//	test_Debug_x64.exe!__acrt_report_runtime_error(const wchar_t * message) 줄 142	C++
	// 	test_Debug_x64.exe!abort() 줄 61	C++
	// 		[외부 코드]
	// 	>	test_Debug_x64.exe!lax::net::session::on_recv_completed(std::error_code & ec, unsigned __int64 len) 줄 190	C++
	// 		test_Debug_x64.exe!lax::net::session::request_recv::__l2::<람다>(std::error_code ec, unsigned __int64 len) 줄 105	C++
	// 		test_Debug_x64.exe!asio::detail::binder2<void <람다>(std::error_code, unsigned __int64), std::error_code, unsigned __int64>::operator()() 줄 128	C++
	// 
	//		- io thread를 디버거에서 잡고 있으면 on_recv_completed에서 위의 에러 나옴
	//      -  

	// - connector failed 발생 : ok 
	//   많은 연결들이 맺어지지 못함
	//   sleep 시간을 늘려서 천천히 진행하니 괜찮음 (100ms)
	//   release 모드에서 괜찮다. accept 해주는 성능과 연관된 것으로 보인다. 
	//   connect 다 처리되기 전에 서비스 종료되어 그런 것으로 보인다.  
}

#endif

TEST_CASE("design send/recv")
{
	SECTION("recv")
	{
		// 역할: 
		// - 고정길이 버퍼 사용. 
		// - 수신한 바이트를 on_recv로 전달. 
		// - 전달하면 사용된 것으로 간주

		// 구현: 
		// - std::array<uint8_t, 32K> buf; 
		// - 받으면 on_recv() 호출하고 상위에 전달하고 재사용 
	}

	SECTION("send")
	{
		// 역할: 
		// - 요청온 바이트 배열을 전송. 
		// - 언제 어떻게 보내는 지는 상위에서 모름 
		

		// 구현: 
		// - segment를 갖는 버퍼를 사용 
		// - segment는 풀링 (concurrent_queue 사용)  
		// - sending_queue로 segment를 갖는 클래스 구현 
	}

	SECTION("research ")
	{
		// 시도했던 것들 

		// buffer: 
		// - 가변 길이 버퍼를 사용
		// - 중간에 메모리 할당 / 해제가 꽤 많은 문제. 
		// - segment를 사용하는 구조 괜찮은 듯 

		// recv : 
		// - 고정 크기 버퍼로 받아서 packetization을 위한 버퍼로 이동 후 처리
		// - 0 recv로 받고 non-blocking으로 가용한 만큼 받아서 처리 

		// send : 
		// - 축적 버퍼와 전송 버퍼를 교대하면서 처리 
		// - 큐에 넣고 처리 할 수도 있을 듯

		// supersocket 
		// recv : 하나의 버퍼를 사용하고 위치를 변경해서 받음. IServerConfig를 통해 버퍼 크기 등 제어 
		// send : SendingQueue를 사용. 여러 개가 있을 경우 BufferList에 지정해서 전송  
	}
}
