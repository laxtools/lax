#include "stdafx.h"

#include <catch.hpp>

#include <deque>
#include <iostream>
#include <shared_mutex>

TEST_CASE("cplus features")
{
	SECTION("union")
	{
		union id
		{
			uint64_t value_ = 0;

			struct
			{
				uint32_t seq_;
				uint16_t index_;
				uint16_t pad_;
			};

			uint64_t value() const
			{
				return value_;
			}
		};

		id v1;

		REQUIRE(v1.seq_ == 0);
		REQUIRE(v1.index_ == 0);

		v1.index_ = 3;
		v1.seq_ = 1;
		v1.pad_ = 0xFFFF;

		REQUIRE(v1.index_ == 3);
		REQUIRE(v1.seq_ == 1);

		std::cout << std::hex << v1.value_ << std::endl;
		std::cout << std::dec << v1.value_ << std::endl;
	}

	SECTION("rvalue ")
	{
		// how to move around? 
		// rvalue�� �ޱ� 
		// ������ �ޱ�

		SECTION("pass by value")
		{
			struct payload
			{
				int v_ = 3;
			};

			struct holder
			{
				holder(payload p)
					: p_(std::move(p))
				{
				}

				payload p_;
			};

			payload p1;

			holder h1(p1);
		}

		SECTION("pass by rvalue reference")
		{

			struct payload
			{
				int v_ = 3;
			};

			struct holder
			{
				holder(payload&& p)
					: p_(std::move(p))
				{
				}

				payload p_;
			};
		}

	}

	SECTION("unicode")
	{
		// string, wstring, char, wchar 
		// windows, osx, linux 
		// osx, linux has utf8 as default with std::string 
		// windows has MBCS char and ucs16 wchar_t  

		SECTION("net")
		{
			// use compatible characters only with std::string and char* 

		}

		SECTION("app - actor")
		{
			// use utf8 to communicate 	
			// use ucs16 for server side handling 
		}
	}

	SECTION("thread safety")
	{
		SECTION("constructor / destructor")
		{
			// �����ڿ� �Ҹ��ڰ� ������ safe�Ѱ�? 

			// ������Ʈ ���� �ֱ��� ��Ȯ�� �и� 
			// - construction 
			// - usage 
			// - destruction 

			// shared_ptr can help 
			// convention / practice / training required
		}
	}

	SECTION("shared_mutex and shared_lock")
	{
		// �ٸ� �����Ϸ� ���� ���� (Ư��, android�� clang ���̺귯��) 

		// ������ ��: 
		// - shared_timed_mutex�� recursive_mutex�� �ƴ� 
		// -  

		SECTION("usage")
		{
			// mutex�� lock. shared_lock�� ��� ���

			std::shared_timed_mutex  mutex;

			int v = 0;

			{
				std::shared_lock<std::shared_timed_mutex> lock(mutex);

				v++;
			}

			REQUIRE(v == 1);

			// shared_timed_mutex�� recursive lock�� �ƴ�
			{
				std::unique_lock<std::shared_timed_mutex> ulock(mutex);

				v++;
			}

			REQUIRE(v == 2);
		}

	
	}
}

__declspec(noinline)
static void func()
{

}

TEST_CASE("safety", "[thread]")
{
	const auto test_count = 2;

	std::shared_timed_mutex  mutex;
	std::deque<int> q;

	int v = 0;
	int sum = 0;

	auto prod_func = []() {};

	/*
	auto prod_func = [&]() {
		for (int i = 0; i < test_count; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			std::unique_lock<std::shared_timed_mutex> lock(mutex);

			v++;

			q.push_back(v);
		}
	};


	auto consume_func = [&]() {

		int run_count = 0;

		while (run_count < test_count * 2)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			std::shared_lock<std::shared_timed_mutex> lock(mutex);

			if (!q.empty())
			{
				sum += q.front();

				lock.unlock();

				// seprate write lock required
				{
					std::unique_lock<std::shared_timed_mutex> ulock(mutex);
					q.pop_front();
				}

				lock.lock();

				++run_count;
			}
		}

		std::cout << sum << std::endl;
	};
	*/

	// �Ʒ� �ڵ尡 ������ ��忡�� access violation�� �߻� �Ѵ�. 

	// ���� 
	// - ����ȭ�� �ϸ� �ζ��� ���ٵ��� ����� �� �ִ� ������ ���δ�.
	//   __declspec(noinline)�� �ص� �� �� 
	// - spdlog ���� ȣ���� ������ �߻�. 
	//   test_logger���� �����ϰ� ���� �� ������.
	// 
	// spdlog 
	// - �񵿱� ��� ���� : fail 
	// - info() �α� �߰� ���� : fail 
	// - log::inst() ȣ�⸸ ������ �߻� 

	std::thread producer1(func);

	/*
	auto producer2 = std::thread(prod_func);

	auto consumer = std::thread(consume_func);

	producer1.join();
	producer2.join();
	consumer.join();

	int sum_count = test_count * 2;

	REQUIRE(sum == (sum_count * (sum_count + 1)) / 2);
	*/

	producer1.join();
}

namespace 
{

struct ref : public std::shared_ptr<ref>
{
	ref(int* v)
		: v_(v)
	{}

	int* operator->()
	{
		return v_;
	}

	int operator*()
	{
		return *v_;
	}

	int* v_ = nullptr;
};

}

TEST_CASE("shared_ptr inheritance")
{
	int v = 3;

	ref r1(&v);

	REQUIRE((*r1) == 3);
}

TEST_CASE("vector iterator")
{
	std::vector<int> vs = { 1, 2, 3, 4 };

	auto it = vs.begin(); 
	std::advance(it, 3);

	REQUIRE(*it == 4);
}

namespace
{

struct holder_base
{
	using ptr = std::shared_ptr<holder_base>;

};

struct holder_a : public holder_base
{
	holder_base::ptr b;


};

struct holder_b : public holder_base
{
	holder_base::ptr a;
};

}

TEST_CASE("shared_ptr reference")
{

	auto aa = std::make_shared<holder_a>();
	auto bb = std::make_shared<holder_b>();

	aa->b = bb;
	bb->a = aa;

	aa.reset();

	auto a_v = std::static_pointer_cast<holder_a>(bb->a);
}

TEST_CASE("multimap erase")
{
	auto mm = std::multimap<int, int>();

	mm.insert(std::multimap<int, int>::value_type(1, 1));
	mm.insert(std::multimap<int, int>::value_type(1, 2));

	mm.erase(1);

	REQUIRE(mm.empty());
}