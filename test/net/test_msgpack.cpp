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
		// msgpack�� C++ ���� ���θ� ����. 
		// �󸶳� ���� ��, ������ ���� ���� �� ���캻��. 

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
		// ������Ʈ�� ũ�� ���ٴ� ���� ������Ʈ�� ������ ���� ���̰� ���� ����. 
		// - 4�� ������Ʈ. 64���� 1�鸸�� pack / unpack�� 16.94 �� �ɸ� 
		// - �ʹ� ������. ��ǥ�� 1�� �̸��̴�. 
		// - ������� ���� ��찡 �������� �ұ��ϰ� ���������� �� ó���Ѵ�. 
		// - Ŭ�󿡼� little endian�� �ƴ� ��츸 ���� ��ȯ�ϰ� 
		// - ���� ��� POD�� ��� �׳� ��ü�� �����ϴ� ������� ó���� �� �ִ� 
		// - �׷� ������ ã�ƾ� �Ѵ�. 
		//
		// ���� ��ü�� ������� ���ٴ� ������ ����
		// - ������� ������ ������ ����. 
		// - �޸� �Ҵ�� ������ ���� ���� ���� 
		// - flatbuffers�� �ڵ�� ���. 
	}

	SECTION("msgpack performance 2")
	{
		// msgpack�� C++ ���� ���θ� ����. 
		// �󸶳� ���� ��, ������ ���� ���� �� ���캻��. 

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

		// ������Ʈ�� ũ�� ���ٴ� ���� ������Ʈ�� ������ ���� ���̰� ���� ����. 
		// - 4�� ������Ʈ. 64���� 1�鸸�� pack / unpack�� 0.697 �� �ɸ�
	}
}