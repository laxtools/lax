#include "stdafx.h"

#include <catch.hpp>
#include <lax/util/state_machine.h>
#include <lax/util/state_lambda.h>

#include <iostream>

using namespace lax::util;

namespace
{
class test_state : public state<int>
{
	typedef state<int> base;

public:
	test_state(int key, state_machine& machine)
		: state<int>(key, machine)
	{
	}

private:
	void on_enter() override {}
	void on_execute() override {}
	void on_exit() override {}
};

}

TEST_CASE("state machine")
{

	SECTION("setup")
	{
		state_machine<int> machine;
		
		machine.add(std::make_shared<test_state>(1, machine));
		machine.add(1, std::make_shared<test_state>(2, machine));

		REQUIRE(machine.find(0));
		REQUIRE(machine.find(0)->has(1));
		REQUIRE(machine.find(0)->has(2));

		REQUIRE(machine.find(1));
		REQUIRE(machine.find(1)->has(2));

		REQUIRE(machine.find(2));

		machine.tran(2);
		machine.execute();	// tran later

		REQUIRE(machine.current()->key() == 2);
		REQUIRE(machine.find(1)->active());
		REQUIRE(machine.find(2)->active());

		machine.tran(1); 
		machine.execute();

		REQUIRE(!machine.find(2)->active());
		REQUIRE(machine.current() == machine.find(1));
	}

	SECTION("transitions")
	{
		state_machine<int> machine;
		
		// 복잡한 트리
		machine.add(std::make_shared<test_state>(1, machine));
		machine.add(std::make_shared<test_state>(2, machine));
		machine.add(std::make_shared<test_state>(3, machine));

		machine.add(1, std::make_shared<test_state>(11, machine));
		machine.add(1, std::make_shared<test_state>(12, machine));
		machine.add(1, std::make_shared<test_state>(13, machine));

		machine.add(2, std::make_shared<test_state>(21, machine));
		machine.add(2, std::make_shared<test_state>(22, machine));
		machine.add(2, std::make_shared<test_state>(23, machine));

		machine.add(3, std::make_shared<test_state>(31, machine));
		machine.add(3, std::make_shared<test_state>(32, machine));
		machine.add(3, std::make_shared<test_state>(33, machine));

		machine.add(31, std::make_shared<test_state>(311, machine));
		machine.add(31, std::make_shared<test_state>(312, machine));
		machine.add(31, std::make_shared<test_state>(313, machine));

		machine.add(32, std::make_shared<test_state>(321, machine));
		machine.add(32, std::make_shared<test_state>(322, machine));
		machine.add(32, std::make_shared<test_state>(323, machine));

		SECTION("simple")
		{
			machine.tran(2);
			machine.execute();

			REQUIRE(machine.current()->key() == 2);
			REQUIRE(machine.find(2)->active());
		}

		SECTION("tran to parent")
		{
			machine.tran(1);
			machine.execute();

			REQUIRE(!machine.find(2)->active());
			REQUIRE(machine.current() == machine.find(1));
		}

		SECTION("tran to child")
		{
			machine.tran(0);
			machine.execute();

			machine.tran(31);
			machine.execute();

			REQUIRE(machine.find(31)->active());
			REQUIRE(machine.find(3)->active());
			REQUIRE(machine.current() == machine.find(31));

			machine.tran(311);
			machine.execute();

			REQUIRE(machine.find(311)->active());
			REQUIRE(machine.find(31)->active());
			REQUIRE(machine.find(3)->active());
			REQUIRE(machine.current() == machine.find(311));
		}

		SECTION("tran far")
		{
			machine.tran(0);
			machine.execute();

			machine.tran(31);
			machine.execute();

			REQUIRE(machine.find(31)->active());
			REQUIRE(machine.find(3)->active());
			REQUIRE(machine.current() == machine.find(31));

			machine.tran(311);
			machine.execute();

			REQUIRE(machine.find(311)->active());
			REQUIRE(machine.find(31)->active());
			REQUIRE(machine.find(3)->active());
			REQUIRE(machine.current() == machine.find(311));

			machine.tran(21);
			machine.execute();

			REQUIRE(!machine.find(311)->active());
			REQUIRE(!machine.find(31)->active());
			REQUIRE(!machine.find(3)->active());

			REQUIRE(machine.find(21)->active());
			REQUIRE(machine.find(2)->active());

			REQUIRE(machine.current() == machine.find(21));
		}
	}

	SECTION("lambda")
	{
		state_machine<int> machine;

		auto s1 = std::make_shared<state_lambda<int>>(1, machine);

		auto fn = s1->set_execute([&s1]() {std::cout << "Hello lambda!" << std::endl;});

		machine.add(s1);
		machine.tran(1, state_machine<int>::transition_option::immediate);

		machine.execute();
		machine.execute();

		REQUIRE(machine.find(1)->stat().execution_count() == 2);
	}
}