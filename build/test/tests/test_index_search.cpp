#include "stdafx.h"

#include <lax/util/macros.h>

#include <array>
#include <catch.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <typeinfo>

namespace
{
struct id
{
	typedef uint16_t key;
	typedef uint8_t level;

	std::array<key, 3> parts_;  // 12 bytes id. 
	level level_ = 0;

	id()
	{
		memset(parts_.data(), 0, parts_.size() * sizeof(key));
	}

	id(key a1)
		: id()
	{
		parts_[0] = a1;
		level_ = 1;
	}

	id(key a1, key a2)
		: id()
	{
		parts_[0] = a1;
		parts_[1] = a2;
		level_ = 2;
	}

	id(key a1, key a2, key a3)
		: id()
	{
		parts_[0] = a1;
		parts_[1] = a2;
		parts_[2] = a3;
		level_ = 2;
	}

	bool operator == (const id& rhs) const
	{
		if (level_ != rhs.level_)
		{
			return false;
		}

		for (std::size_t i = 0; i < parts_.size(); ++i)
		{
			if (parts_[i] != rhs.parts_[i])
			{
				return false;
			}
		}

		return true;
	}

	bool operator < (const id& rhs) const
	{
		for (std::size_t i = 0; i < parts_.size(); ++i)
		{
			if (parts_[i] < rhs.parts_[i])
			{
				return true;
			}
		}

		return false;
	}

	bool is_child(const id& rhs) const
	{
		for (std::size_t i = 0; i < parts_.size(); ++i)
		{
			if (parts_[i] != 0 && parts_[i] == rhs.parts_[i])
			{
				return true;
			}
		}

		return false;
	}
};
}

namespace std
{
// hash function for id. unordered_map에서 선언 전에 필요

template <>
struct hash<id>
{
	std::size_t operator()(const id& i) const
	{
		auto hasher = std::hash<uint16_t>(); 

		auto v1 = hasher(i.parts_[0]); 
		auto v2 = hasher(i.parts_[1]); 
		auto v3 = hasher(i.parts_[2]); 

		std::size_t res = 17; 

		res = res * 31 + v1; 
		res = res * 31 + v2; 
		res = res * 31 + v3;

		return res;
	}
};

} // std

namespace 
{
struct node
{
	id			key_ = id();
};

struct hash_tree 
{
	typedef std::unique_ptr<node> node_ptr;
	typedef std::unordered_map<id, node_ptr> node_map;
	typedef std::vector<id> id_list;

	id			key_ = id();
	node_map	nodes_;
	id_list		childs_;

	node& add(node_ptr n)
	{
		// nodes_.insert(node_map::value_type(n->key_, n));
	}

	node& parent(node_ptr node)
	{

	}

	node& parent()
	{

	}
};

} // noname 


TEST_CASE("node map")
{
	SECTION("hash test")
	{
		id i;
		i.parts_[0] = 1;
		i.parts_[1] = 2;
		i.parts_[2] = 1;

		id j;
		j.parts_[0] = 2;
		j.parts_[1] = 1;
		j.parts_[2] = 3;

		auto hasher = std::hash<id>();

		REQUIRE(hasher(i) != hasher(j));
	}

	SECTION("hashmap test")
	{
		// node::node_map nodes; 

		id i;
		i.parts_[0] = 1;
		i.parts_[1] = 0;
		i.parts_[2] = 0;

		// nodes[i] = std::make_unique<node>();

		// 루트 레벨 노드 
		// 2 / 1 / 3 이라고 하면 부모는 2 / 1 / 0 
		// 조부모는 2 / 0 / 0 
		// 위와 같이 전체 노드를 갖고 있고 
		// 부모를 쉽게 찾아가는 방식

		// hash map 하나로 전체 관리가 가능
		// tag / 특성별 indexer를 두고 추가 검색 가능하게 함 

		// 가볍고 빠른 구조를 찾았다. 
	}

}