#include "stdafx.h"

#include <catch.hpp>

#include <unordered_map>

template <class Key, class Value>
struct HashIndexer
{
	using container = std::unordered_map<Key, Value>;

	struct result
	{
		result() = default;

		result(const Value& value)
			: value_(value)
			, valid_(true)
		{
		}

		bool valid() const
		{
			return valid_;
		}

		const Value& value() const
		{
			return value_;
		}

		Value			null_  = Value();
		const Value&	value_ = null_;  
		bool			valid_ = false;
	};

	container map_;
	
	void link(const Key& key, const Value& value)
	{
		map_[key] = value;
	}

	void unlink(const Key& key)
	{
		map_.erase(key);
	}

	result find(const Key& key)
	{
		auto iter = map_.find(key);

		if (iter == map_.end())
		{
			return result();
		}

		return result(iter->second);
	}
};

TEST_CASE("design a general indexer")
{

	SECTION("hashmap")
	{
		SECTION("simple interface")
		{
			HashIndexer<int, std::string> indexer;

			indexer.link(1, "hello");

			REQUIRE(indexer.find(1).valid());
			REQUIRE(indexer.find(1).value() == "hello");
		}


		SECTION("performance")
		{
			const int testCount = 100'000;

			HashIndexer<int, int> indexer;

			for (int i = 0; i < testCount; ++i)
			{
				indexer.link(i, i);
			}

			for (int i = 0; i < testCount; ++i)
			{
				indexer.find(i);
			}

			for (int i = 0; i < testCount; ++i)
			{
				indexer.unlink(i);
			}
		}
	}

}