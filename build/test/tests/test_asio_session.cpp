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
	// acceptor / connector ����ؼ� �پ��� ��Ʈ�� session�� �ΰ� 
	// ������ �����ϴ� ������ ���� �����Ѵ�.  

	SECTION("acceptor / connector")
	{
		// ������ �ΰ� ���´�. 
		// acceptor�� ���� Ŭ������? 
	}

	SECTION("session management")
	{
		// �ξ��� ������ ����. ���� �������̽� ���
		// �Ϲ�ȭ �ϱ� ���� SuperSocketó�� session���� ��� �޵��� �Ѵ�. 
	}

	SECTION("send")
	{
		// queue�� �ְ� �ѹ��� ����
		// text�� ���������� ó��
		// vector ������ async_send ���. 32K ������ ť ��� ����.  
	}

	SECTION("recv")
	{
		// proactor ���ƿ��� ���� ť�� ����
	}
}