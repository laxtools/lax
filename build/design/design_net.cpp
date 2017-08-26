#include "stdafx.h"

#include <catch.hpp>
#include <future>
#include <asio.hpp>

TEST_CASE("design net")
{
	SECTION("asio")
	{
		SECTION("acceptor")
		{
		}

		SECTION("connector")
		{
			asio::io_service ios; 

		
		}

		SECTION("echo")
		{
		}
	}

	SECTION("thinking communication")
	{
		SECTION("listen / connect ")
		{
			// 누군가 듣는 사람과 말하는 사람이 있어야 한다. 
			// 항상 듣고 말하는 관계를 유지하기는 어렵다.

			// tcp 소켓 상태와 프로토콜이 이를 잘 보여준다. 
		}

		SECTION("open / negotiate / send / recv / close")
		{

		}
	}
}