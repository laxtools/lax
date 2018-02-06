#include "stdafx.h"
#include <catch.hpp>
#include <lax/util/json.hpp>
#include <iostream>
#include <sstream>

namespace
{
struct tv
{
	int k = 3;
	std::string h = "hello";
};
} // noname

TEST_CASE("test json usage")
{
	// switch to nlohmann's json 
	// - supports serialization
	// - supports binary format (efficient loading without parsing) 

	// rapid json is faster, but features are better. 
	// - supports binary formats of UBJSON, msgpack. 
	// - this will make loading speed faster.

	SECTION("basic usage")
	{
		// or even nicer with a raw string literal
		try
		{
			auto j2 = R"({ "happy": true, "pi": 3.141 })"_json;

			CHECK(j2["happy"] == true);

			// debugging:
			// - can watch from m_type and m_value 
			// - easier than dropbox's

			auto pi = j2["pi"];

			CHECK(pi.get<float>() == 3.141f);
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

		//
		// iterator for arrays
		// more functions
		// stream support 
		// 
		// utf8, unicode16LE, 32LE support 
	}

	SECTION("array")
	{
		auto j2 = R"([ 
						[ 1, 2, 3, 4, 5],
						[ 2, 3, 4, 5, 6] 
					 ])"_json;

		// CHECK(j2[0][1] == 1); access error. why?

		auto a1 = j2[0];
		auto a11 = a1[2];
		CHECK(a11 == 3);

		CHECK(a1.size() == 5);

		for (auto& v : a1)
		{
			std::cout << "v: " << v << " ";
		}

		std::cout << std::endl;
	}

	SECTION("stream support")
	{
		auto str = R"([ 
						[ 1, 2, 3, 4, 5],
						[ 2, 3, 4, 5, 6] 
					 ])";

		std::istringstream is(str);

		nlohmann::json js;

		is >> js; // can throw exception

		auto a1 = js[0];
		auto a11 = a1[2];
		CHECK(a11 == 3);

		CHECK(a1.size() == 5);
	}
}


//
// History Section:
// - dropbox json
//


//TEST_CASE("test json usage")
/*
void TEST_NONE()
{

	SECTION("parse from string and usage")
	{
		SECTION("simple case")
		{
			std::string err;

			// key value
			{
				auto res = json::Json::parse("{ \"key\": 33 }", err);

				REQUIRE(err.empty());

				auto& v1 = res["key"];

				REQUIRE(v1.is_number());
				REQUIRE(v1.int_value() == 33);
			}

			// array
			{
				auto res = json::Json::parse("{ \"key\": [1, 2, 3] }", err);

				REQUIRE(err.empty());

				auto& arr1 = res["key"];
				REQUIRE(arr1.is_array());
				REQUIRE(arr1[0].is_number());
				REQUIRE(arr1[0].int_value() == 1);
			}
		}

		SECTION("object case")
		{
			std::string err;

			auto res = json::Json::parse(R"({ "key": { "key2" : ["abc", "cde"] } })", err);

			REQUIRE(err.empty());

			// R"()"°¡ ¹®¹ý
			auto o1 = res["key"];
			REQUIRE(o1.is_object());

			auto k2 = o1["key2"];
			REQUIRE(k2.is_array());

			REQUIRE(k2[0].string_value() == "abc");


			std::cout << k2.dump() << std::endl;

			// debugging practice:
			// - use err returned from parse to log 
			// - use dump to debug the string

			auto k2_1 = k2[1];

			REQUIRE(k2_1.string_value() == "cde");

			// using watch to debug: 
			// - casting to actual type 

			// - get ptr address
			//
			//  k2_1{ m_ptr = shared_ptr{ ... }[0x00000002 strong refs][make_shared] }	json::Json
			//	- m_ptr	shared_ptr{ ... }[0x00000002 strong refs][make_shared]	std::shared_ptr<json::JsonValue>
			//	-[ptr]	0x000001e1460f5220 {...}	json::JsonValue *{ json::JsonString }

			// - cast to JsonString* to get m_value
			//
			// (json::JsonString*)0x000001e1460f5220	0x000001e1460f5220 {...}	json::JsonString *
			//	-json::Value<3, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >	{m_value = "cde" }	json::Value<3, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >
			//	+json::JsonValue{ ... }	json::JsonValue
			//	+ m_value	"cde"	const std::basic_string<char, std::char_traits<char>, std::allocator<char> >
		}
	}

	SECTION("serialize")
	{
		tv value;

		auto v1 = json::Json(value.k);
		auto v2 = json::Json(value.h);

		json::Json::object o;

		o["k"] = v1;
		o["h"] = v2;

		auto f = json::Json(o);

		std::cout << f.dump() << std::endl;
	}

	SECTION("analysis")
	{
		// Json
		// JsonValue
		// - internal

		// Json
		// - from types
		// - from type::to_json
		// - dump to string
		// - parse from string
		// Implementation
		// - values inheriting JsonValue 
		//   - a kind of pimpl to hide details
		// - statics 
		//   - a useful global holder technique
		// - parsing
		//   - top down recursive parsing
	}
}
*/