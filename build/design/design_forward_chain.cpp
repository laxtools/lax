#include "stdafx.h"

#include <catch.hpp>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace
{
struct msg
{
	int id_ = 0;

	// todo : flag bit field for dispatching 
	// todo : dest, src field to filter delivery

	msg() = default;
	msg(int id)
		: id_(id)
	{
	}

	int id() const
	{
		return id_;
	}
};

struct actor
{
	virtual void on_msg(const msg& m)
	{
		++recv_;
	}

	int recv_;
};

using actor_ptr = std::shared_ptr<actor>;

struct proto
{
	using subs = std::vector<actor_ptr>;
	using container = std::unordered_map<int, subs>;

	void sub(int id, actor_ptr ptr)
	{
		if (has(id, ptr))
		{
			return;
		}

		std::unique_lock<std::shared_timed_mutex> lock(mtx_);

		auto iter = map_.find(id);

		if (iter == map_.end())
		{
			subs subsl;
			subsl.push_back(ptr);

			map_.emplace(id, subsl);
		}
		else
		{
			iter->second.push_back(ptr);
		}
	}

	void unsub(int id, actor_ptr ptr)
	{
		std::unique_lock<std::shared_timed_mutex> lock(mtx_);

		auto iter = map_.find(id);

		if (iter != map_.end())
		{
			iter->second.erase(
				std::remove_if(
					iter->second.begin(),
					iter->second.end(),
					[&ptr](actor_ptr p) {return ptr.get() == p.get();}
					),
				iter->second.end());
		}
	}

	void post(const msg& m)
	{
		std::shared_lock<std::shared_timed_mutex> lock(mtx_);

		auto iter = map_.find(m.id());

		if (iter != map_.end())
		{
			for (auto& aptr : iter->second)
			{
				aptr->on_msg(m);
			}
		}
	}

	bool has(int id, actor_ptr ptr)
	{
		std::shared_lock<std::shared_timed_mutex> lock(mtx_);

		auto iter = map_.find(id);

		if (iter != map_.end())
		{
			for (auto& aptr : iter->second)
			{
				if (aptr.get() == ptr.get())
				{
					return true;
				}
			}
		}

		return false;
	}

	container map_;
	std::shared_timed_mutex mtx_;
};

} // noname


TEST_CASE("design forward chain")
{
	SECTION("sub ")
	{
		proto p; 

		actor_ptr a1(new actor());
		actor_ptr a2(new actor());

		p.sub(1, a1);
		p.sub(1, a2);
		p.sub(2, a2);

		p.post(msg(1));
		REQUIRE(a1->recv_ == 1);
		REQUIRE(a2->recv_ == 1);

		p.post(msg(2));
		REQUIRE(a1->recv_ == 1);
		REQUIRE(a2->recv_ == 2);
	}

	SECTION("flag filtering")
	{

	}

}