#include "stdafx.h"

#include <catch.hpp>

class dir
{
public: 
	struct id
	{
		static constexpr int invalid = 0;

		int depth = 0;
	};

	template <int N = 3>
	struct id_n : public id
	{
		uint16_t ids_[N];

		id_n()
		{
			for (int i = 0; i < N; ++i)
			{
				ids_[i] = invalid;
			}
		}

		int depth = N;
	};

	struct entry 
	{
		std::string name = "none";
	};

public: 

	void add(id& id, entry e)
	{
	}
		

	bool find(const id& id, entry& e)
	{
	}
};

TEST_CASE("design tree index")
{
	// 목표는 가벼운 계층 아이디와 빠른 검색을 만드는 것이다. 

	

}

