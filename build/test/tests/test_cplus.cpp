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
		// rvalue로 받기 
		// 값으로 받기

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
			// 생성자와 소멸자가 쓰레드 safe한가? 

			// 오브젝트 생명 주기의 명확한 분리 
			// - construction 
			// - usage 
			// - destruction 

			// shared_ptr can help 
			// convention / practice / training required
		}
	}

	SECTION("shared_mutex and shared_lock")
	{
		// 다른 컴파일러 지원 여부 (특히, android의 clang 라이브러리) 

		// 주의할 점: 
		// - shared_timed_mutex는 recursive_mutex가 아님 
		// -  

		SECTION("usage")
		{
			// mutex와 lock. shared_lock의 사용 방법

			std::shared_timed_mutex  mutex;

			int v = 0;

			{
				std::shared_lock<std::shared_timed_mutex> lock(mutex);

				v++;
			}

			REQUIRE(v == 1);

			// shared_timed_mutex는 recursive lock이 아님
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

	// 아래 코드가 릴리스 모드에서 access violation이 발생 한다. 

	// 추적 
	// - 최적화를 하면 인라인 람다들이 사라질 수 있는 것으로 보인다.
	//   __declspec(noinline)을 해도 안 됨 
	// - spdlog 관련 호출이 있으면 발생. 
	//   test_logger에서 제거하고 빌드 후 괜찮음.
	// 
	// spdlog 
	// - 비동기 모드 제거 : fail 
	// - info() 로그 추가 제거 : fail 
	// - log::inst() 호출만 있으면 발생 

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