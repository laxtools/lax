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
		// ���� directory ���� 
		// - asio/include 

		// non-boost ���� 
		// - ASIO_STANDALONE ��ũ�� ���� 

		// acceptor_.async_accept(socket_, lambda ) 
		// - intellisense fails. builds ok. 
		// - IDE���� ������ ���̴� �� �� ����
		// - ���� ��� ȯ���� ���� �������� C++ ����� �ʿ��� (������)

		// build / run �� ��
	}

	SECTION("socket basics")
	{
		SECTION("echo example")
		{
			asio::io_service io_service;

			server s(io_service, 7777);

			// io_service.run();

			// [1] acceptor
			// - �ڱ� listen socket�� ����
			// - �⺻���� �ִ� ���� ������ŭ ó�� 
			// - backlog�� ��, accept ��� ������ ���� ��Ȯ���� ���� 

			// [2] session 
			// - auto self(shared_from_this());
			// - do_read(), do_write() ȣ�⿡�� ������ ����
			// - ������ ���� �ۼ����δ� ������ ���

			// [3] std::move
			// - ������ �����ϰ� std::move�� ����ϴ� ��� 
			// - �پ��� �������̽� �ʿ� ���� �� �����ϰ� ����


			// session�� socket�� accept�� socket�� ���� 
			// 
		}
	}
}