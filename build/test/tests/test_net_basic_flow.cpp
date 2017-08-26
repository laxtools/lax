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

	service::creator func = [] // vs 2015 intellisense ����
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
	// - creator�� connector�� acceptor���� empty�� ����. reference���� ����� ���� : ok
	// - on_connected���� ���� ���� �ϴ� �κ� ���� �� �� ���� : ok 

}

#if 1
TEST_CASE("net connection test")
{
	lax::test::test_app ap;

	service svc(&ap);

	service::creator func = [] // vs 2015 intellisense ����
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
	// - on_recv_completed �߿� runtime ���� �߻� 
	//  test_Debug_x64.exe!issue_debug_notification(const wchar_t * const message) �� 125	C++
	//	test_Debug_x64.exe!__acrt_report_runtime_error(const wchar_t * message) �� 142	C++
	// 	test_Debug_x64.exe!abort() �� 61	C++
	// 		[�ܺ� �ڵ�]
	// 	>	test_Debug_x64.exe!lax::net::session::on_recv_completed(std::error_code & ec, unsigned __int64 len) �� 190	C++
	// 		test_Debug_x64.exe!lax::net::session::request_recv::__l2::<����>(std::error_code ec, unsigned __int64 len) �� 105	C++
	// 		test_Debug_x64.exe!asio::detail::binder2<void <����>(std::error_code, unsigned __int64), std::error_code, unsigned __int64>::operator()() �� 128	C++
	// 
	//		- io thread�� ����ſ��� ��� ������ on_recv_completed���� ���� ���� ����
	//      -  

	// - connector failed �߻� : ok 
	//   ���� ������� �ξ����� ����
	//   sleep �ð��� �÷��� õõ�� �����ϴ� ������ (100ms)
	//   release ��忡�� ������. accept ���ִ� ���ɰ� ������ ������ ���δ�. 
	//   connect �� ó���Ǳ� ���� ���� ����Ǿ� �׷� ������ ���δ�.  
}

#endif

TEST_CASE("design send/recv")
{
	SECTION("recv")
	{
		// ����: 
		// - �������� ���� ���. 
		// - ������ ����Ʈ�� on_recv�� ����. 
		// - �����ϸ� ���� ������ ����

		// ����: 
		// - std::array<uint8_t, 32K> buf; 
		// - ������ on_recv() ȣ���ϰ� ������ �����ϰ� ���� 
	}

	SECTION("send")
	{
		// ����: 
		// - ��û�� ����Ʈ �迭�� ����. 
		// - ���� ��� ������ ���� �������� �� 
		

		// ����: 
		// - segment�� ���� ���۸� ��� 
		// - segment�� Ǯ�� (concurrent_queue ���)  
		// - sending_queue�� segment�� ���� Ŭ���� ���� 
	}

	SECTION("research ")
	{
		// �õ��ߴ� �͵� 

		// buffer: 
		// - ���� ���� ���۸� ���
		// - �߰��� �޸� �Ҵ� / ������ �� ���� ����. 
		// - segment�� ����ϴ� ���� ������ �� 

		// recv : 
		// - ���� ũ�� ���۷� �޾Ƽ� packetization�� ���� ���۷� �̵� �� ó��
		// - 0 recv�� �ް� non-blocking���� ������ ��ŭ �޾Ƽ� ó�� 

		// send : 
		// - ���� ���ۿ� ���� ���۸� �����ϸ鼭 ó�� 
		// - ť�� �ְ� ó�� �� ���� ���� ��

		// supersocket 
		// recv : �ϳ��� ���۸� ����ϰ� ��ġ�� �����ؼ� ����. IServerConfig�� ���� ���� ũ�� �� ���� 
		// send : SendingQueue�� ���. ���� ���� ���� ��� BufferList�� �����ؼ� ����  
	}
}
