#include "stdafx.h"

#include <catch.hpp>
#include <iostream>
#include <functional>

// IDEAS - tag to include integer field value

// NOTE: History code to build ideas.  

namespace
{

struct msg
{
	int key;
	int iv; 
	float fv; 

};

/// �ڵ� ���� �����
template <class Type>
struct evaluator
{
	using func = std::function <Type(msg&)>;

	func func_;
};

/// a function to lookup a field
/**
* field �� ������� subscribe �ϱ� ���� ���
*/
struct lookup
{
	enum type
	{
		esv,
		ei8v,
		ei16v,
		ei32v,
		ei64v,
		eui8v,
		eui16v,
		eui32v,
		eui64v,
		efv,
		edv,
		ecv,
	};

	type type_;

	evaluator<int> eval_int_;

	lookup(type atype)
		: type_(atype)
	{}

	template <class Type>
	Type eval()
	{
		return Type();
	}
};

template <class Type> 
bool compare(Type v1, Type v2)
{
	return v1 == v2;
}

struct msg_derived : public msg
{
	msg_derived()
	{
		key = 3;
	}

	std::string sv;
};

void eval(msg* a)
{
	auto m2 = static_cast<msg_derived*>(a);

	compare(m2->sv, std::string("hello"));
}

struct post 
{
	// � �����͸� ������ �ϴ°�? 

	void dispatch(msg* m)
	{
		// type casting
		dispatch_field_string(m);
	}

	void dispatch_field_string(msg* m)
	{
		auto m2 = static_cast<msg_derived*>(m);

		// setup somewhere
		string_table["hello"] = 3;
		
		auto iter = string_table.find(m2->sv);

		if (iter != string_table.end())
		{
			std::cout << "found refs for " << m2->sv << std::endl;
		}
	}

	std::map<std::string, int> string_table;
};

struct tag_field
{
	int index; 
	int iv;
};


struct post2
{
	using itable = std::map<int, int>;

	// 
	using extractor = std::function<void(itable& itv, msg*)>;

	std::map<int, extractor> extractors;

	itable itable_;

	void dispatch(msg* m)
	{
		auto iter = extractors.find(m->key);

		if (iter != extractors.end())
		{
			iter->second(itable_, m);
		}
	}
};

} // noname

TEST_CASE("field lookup design")
{
	// Ÿ�� ������ ���� ���� ã�Ƽ� ���ϴ� ��� 
	// 
	//  - �̰� �Ǹ� ���� ���� �� �� ���� �� �ѵ�  
	//  -   

	// 
	SECTION("idea ")
	{
		post p; 

		msg_derived m; 
		
		m.sv = "hello";

		p.dispatch(&m);
	}

	SECTION("idea - post2")
	{
		post2 p2;

		post2::extractor ext = [](post2::itable& itv, msg* m) {
			
			auto m2 = static_cast<msg_derived*>(m);

			auto iter = itv.find(m2->iv);

			// get ref and post or 
			// call function if has function 
		};

		p2.extractors[3] = ext;

		msg_derived m2;

		p2.dispatch(&m2);
	}
}