#include "stdafx.h"

#include <catch.hpp>
#include <asio.hpp>

using asio::ip::tcp;

namespace 
{
class server
{
public:
	server(asio::io_service& io_service, short port)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
		, socket_(io_service) // intellisense fails
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](std::error_code ec)
		{
			if (!ec)
			{
				// std::make_shared<session>(std::move(socket_))->start();
			}

			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};
} // noname

TEST_CASE("asio session")
{
	// acceptor / connector 사용해서 다양한 포트로 session을 맺고 
	// 세션을 관리하는 구조에 대해 정리한다.  

	SECTION("acceptor / connector")
	{
		// 연결을 맺고 끊는다. 
		// acceptor를 별도 클래스로? 
	}

	SECTION("session management")
	{
		// 맺어진 연결을 관리. 상위 인터페이스 고려
		// 일반화 하기 위해 SuperSocket처럼 session에서 상속 받도록 한다. 
	}

	SECTION("send")
	{
		// queue에 넣고 한번에 전송
		// text도 마찬가지로 처리
		// vector 버전의 async_send 사용. 32K 블럭들의 큐 방식 버퍼.  
	}

	SECTION("recv")
	{
		// proactor 문맥에서 만들어서 큐로 전송
	}
}