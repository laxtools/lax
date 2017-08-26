#include "stdafx.h"

#include <lax/util/json.hpp>
#include <catch.hpp>
#include <iostream>

using namespace json;

TEST_CASE("json 11")
{
	SECTION("usage")
	{
		SECTION("from string to json")
		{
			const std::string test = R"({"k1":"v1", "k2":42, "k3":["a", 123, true, false, null]})";

			std::string err; 

			const auto json = json::Json::parse(test, err);

			REQUIRE(json["k1"].string_value() == "v1");
			REQUIRE(json["k3"][0].string_value() == "a");
			REQUIRE(json["k3"][1].int_value() == 123);
		}

		SECTION("from object to string")
		{
			const Json obj = Json::object({
				{"k1", "v1"}, 
				{"k2", 42}, 
				{"k3", Json::array({"a", 123.0, true, false, nullptr})}
			});

			std::cout << obj.dump() << std::endl;
		}

		SECTION("check ")
		{
			const Json obj = Json::object({
				{"k1", "v1"}, 
				{"k2", 42}, 
				{"k3", Json::array({"a", 123.0, true, false, nullptr})}
			});

			REQUIRE(obj["k4"].is_null());
		}

		SECTION("invalid field")
		{
			const Json obj = Json::object({
				{ "k1", "v1" },
				{ "k2", 42 },
				{ "k3", Json::array({ "a", 123.0, true, false, nullptr }) }
			});

			obj["invalid"].int_value();
		}
	}
}
