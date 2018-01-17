#include "stdafx.h"
#include <catch.hpp>
#include <msgpack.hpp>
#include <lax/util/simple_timer.hpp>
#include <iostream>

namespace
{

class A
{
public: 
	static int v;

};

class B : public A
{

};

int A::v = 0;

class myclass 
{
public:
	std::string str_;
	std::vector<int> vec_;

public:
	MSGPACK_DEFINE(str_, vec_);
};

} // noname


TEST_CASE("msgpack protocol")
{
	SECTION("c++ static variable")
	{
		A::v = 3;

		REQUIRE(B::v == 3);

		// to check session::channel_ is shared for all subclasses
	}

	SECTION("msgpack usage interface")
	{
		std::vector<myclass> vec;

		for (int i = 0; i < 1024; ++i)
		{
			vec.emplace_back(myclass());
			vec.back().str_ = "hello world!";
			vec.back().vec_.push_back(1);
			vec.back().vec_.push_back(2);
			vec.back().vec_.push_back(3);
		}

		msgpack::sbuffer sbuf;
		msgpack::pack(sbuf, vec);

		msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

		msgpack::object obj = oh.get();

		// you can convert object to myclass directly
		std::vector<myclass> rvec;
		obj.convert(rvec);


		// how it works? 

		// MSGPACK_DEFINE 
		// - 
		// pack 
		// unpack
		// convert
	}

	SECTION("msgpack performance 1")
	{
		// msgpack의 C++ 구현 내부를 본다. 
		// 얼마나 빠른 지, 개선할 점은 없는 지 살펴본다. 

		constexpr int vec_size = 1;

		std::vector<myclass> vec;

		for (int i = 0; i < vec_size; ++i)
		{
			vec.emplace_back(myclass());
			vec.back().str_ = "hello world!";
			vec.back().vec_.push_back(1);
			vec.back().vec_.push_back(2);
			vec.back().vec_.push_back(3);
		}

		constexpr int test_count = 1000000;

		lax::util::simple_timer timer;

		for (int i = 0; i < test_count; ++i)
		{
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, vec);

			msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

			msgpack::object obj = oh.get();

			// you can convert object to myclass directly
			std::vector<myclass> rvec;
			obj.convert(rvec);
		}

		std::cout << "msgpack: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

		//
		// 오브젝트의 크기 보다는 개별 오브젝트의 개수에 따라 차이가 많이 난다. 
		// - 4개 오브젝트. 64개의 1백만번 pack / unpack에 16.94 초 걸림 
		// - 너무 느리다. 목표는 1초 미만이다. 
		// - 엔디안이 같은 경우가 많음에도 불구하고 개별적으로 다 처리한다. 
		// - 클라에서 little endian이 아닐 경우만 개별 변환하고 
		// - 같은 경우 POD일 경우 그냥 전체를 복사하는 방식으로 처리할 수 있는 
		// - 그런 포맷을 찾아야 한다. 
		//
		// 포맷 자체의 문제라기 보다는 구현의 문제
		// - 개념상의 포맷은 문제가 없다. 
		// - 메모리 할당과 해제가 많은 것이 문제 
		// - flatbuffers가 코드는 깔끔. 
	}

	SECTION("msgpack performance 2")
	{
		// msgpack의 C++ 구현 내부를 본다. 
		// 얼마나 빠른 지, 개선할 점은 없는 지 살펴본다. 

		constexpr int vec_size = 1;

		std::vector<myclass> vec;

		for (int i = 0; i < 1; ++i)
		{
			vec.emplace_back(myclass());
			vec.back().str_ = "hello world!";
			vec.back().vec_.push_back(1);
			vec.back().vec_.push_back(2);
			vec.back().vec_.push_back(3);
		}

		constexpr int test_count = 1;

		lax::util::simple_timer timer;

		for (int i = 0; i < test_count; ++i)
		{
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, vec);

			msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

			msgpack::object obj = oh.get();

			// you can convert object to myclass directly
			std::vector<myclass> rvec;
			obj.convert(rvec);
		}

		std::cout << "msgpack: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

		// 오브젝트의 크기 보다는 개별 오브젝트의 개수에 따라 차이가 많이 난다. 
		// - 4개 오브젝트. 64개의 1백만번 pack / unpack에 0.697 초 걸림
	}
}