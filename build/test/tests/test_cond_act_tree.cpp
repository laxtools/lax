#include "stdafx.h"

#include <catch.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>

namespace
{

struct act
{
	using ptr = act*;
	using vec = std::vector<ptr>;

	enum status
	{
		success, 
		failure, 
		running
	};

	act(const std::string& name)
		: name_(name)
	{
	}

	virtual ~act() {}

	virtual status on_tick() = 0;

	// on_event�� ��� ������ ó���ϰ� �� ���带 ���� �˸��� ���� ���ڴ�. 	

	std::string name_;
};

struct composite : public act
{
	vec acts;
	act* current = nullptr;		// running act

	composite(const std::string& name)
		: act(name)
	{
	}

	template <typename Node>
	act& add()
	{
		return add(new Node);
	}

	act& add(ptr p)
	{
		acts.push_back(p);
		return *this;
	}
};

struct sequence : public composite
{
	sequence(const std::string& name)
		: composite(name)
	{
	}

	virtual act::status on_tick() override
	{
		for (auto& p : acts)
		{
			auto rc = p->on_tick(); 

			if (rc != status::success)
			{
				return rc; // ������ �ƴ϶�� ����
			}
		}

		return status::failure; // �� �����ϸ� failure
	}
};


struct selector : public composite
{
	selector (const std::string& name)
		: composite(name)
	{
	}

	virtual act::status on_tick() override
	{
		if (current)
		{
			auto rc = current->on_tick();

			if (rc != status::failure)
			{
				return rc;
			}

			current = nullptr;
		}

		for (auto& p : acts)
		{
			auto rc = p->on_tick();

			if (rc != status::failure)
			{
				current = p;

				return rc; // ���а� �ƴ϶�� ����
			}
		}

		return act::status::failure; // �� �����ϸ� failure
	}
};

struct lambda : public act
{
	using func_tick = std::function<status ()>;

	func_tick tick_;

	lambda(const std::string& name, func_tick ftick)
		: act(name)
		, tick_(ftick)
	{
	}

	virtual status on_tick() override
	{
		return tick_();
	}
};

// to provide fluent api style building
struct act_builder
{
	
};

}

TEST_CASE("act tree")
{

}
