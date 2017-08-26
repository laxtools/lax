#include "stdafx.h"

#include <lax/util/macros.h>

#include <catch.hpp>
#include <memory>
#include <iostream>

struct simple
{
	virtual void one() {}
};

struct derived : public simple
{
	virtual void one() override {}
};

static void send(std::shared_ptr<simple> msg)
{
	msg.get();

	// must be 2
	std::cout << "use_count: " << msg.use_count() << std::endl;
}

TEST_CASE("shared ptr conversion")
{
	SECTION("conversion")
	{
		auto ptr = std::make_shared<derived>();

		{
			std::shared_ptr<simple> ptr2 = ptr;
			REQUIRE(ptr.use_count() == 2);
		}

		REQUIRE(ptr.use_count() == 1);

		// event 포인터 wrapping 수정해서 가능할 듯
	}
	
	SECTION("implicit conversion")
	{
		auto ptr = std::make_shared<derived>();

		send(ptr);

		REQUIRE(ptr.use_count() == 1);

		// event 포인터 wrapping 수정해서 가능할 듯
	}
}