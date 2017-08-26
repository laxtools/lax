#include "stdafx.h"

#include <catch.hpp>
#include <asio.hpp>

using asio::ip::tcp;

namespace
{
class session
	: public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket)
		: socket_(std::move(socket)) // intellisense fails
	{
	}

	void start()
	{
		do_read();
	}

private:
	void do_read()
	{
		auto self(shared_from_this());
		socket_.async_read_some(asio::buffer(data_, max_length),
			[this, self](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				do_write(length);
			}
		});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		asio::async_write(socket_, asio::buffer(data_, length),
			[this, self](std::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				do_read();
			}
		});
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

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
				std::make_shared<session>(std::move(socket_))->start();
			}

			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};
} // noname

TEST_CASE("asio ")
{
	SECTION("building")
	{
		// 포함 directory 설정 
		// - asio/include 

		// non-boost 지정 
		// - ASIO_STANDALONE 매크로 정의 

		// acceptor_.async_accept(socket_, lambda ) 
		// - intellisense fails. builds ok. 
		// - IDE에서 오류로 보이는 건 좀 문제
		// - 최종 사용 환경은 보다 전통적인 C++ 사용이 필요함 (아직은)

		// build / run 잘 됨
	}

	SECTION("socket basics")
	{
		SECTION("echo example")
		{
			asio::io_service io_service;

			server s(io_service, 7777);

			// io_service.run();

			// [1] acceptor
			// - 자기 listen socket을 포함
			// - 기본으로 최대 소켓 개수만큼 처리 
			// - backlog인 지, accept 등록 개수인 지는 명확하지 않음 

			// [2] session 
			// - auto self(shared_from_this());
			// - do_read(), do_write() 호출에서 참조를 유지
			// - 간단한 예제 작성으로는 괜찮은 방법

			// [3] std::move
			// - 값으로 전달하고 std::move를 사용하는 방식 
			// - 다양한 인터페이스 필요 없을 때 간단하게 가능


			// session의 socket과 accept한 socket의 관계 
			// 
		}
	}
}