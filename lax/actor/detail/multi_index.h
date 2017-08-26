#pragma once 

#include <lax/util/result.h>
#include <map>
#include <unordered_map>
#include <vector>

namespace lax
{
namespace actor
{

/// index for directory 
/**
 * 여러 Link를 갖는 인덱스. multi_index 
 * 
 * NOTE: find가 자주 호출되는 경우 느릴 수 있음
 * NOTE: Link의 복사 성능 주의
 */
template <class Key, class Link>
class multi_index
{
public:
	using vec = std::vector<Link>;

	multi_index() = default;
	~multi_index() = default;

	bool link(Key key, Link link)
	{
		auto iter = links_.insert(map::value_type(key, link));

		return iter != links_.end(); // duplicate
	}

	bool find(Key key, vec& res) const
	{
		auto lb = links_.lower_bound(key);
		auto ub = links_.upper_bound(key);

		for (auto iter = lb; iter != ub; ++iter)
		{
			res.push_back(iter->second);
		}

		return !res.empty(); // not found
	}

	bool has(Key key) const
	{
		return links_.find(key) != links_.end();
	}

	bool unlink(Key key, Link link)
	{
		auto lb = links_.lower_bound(key);
		auto ub = links_.upper_bound(key);

		for (auto iter = lb; iter != ub; ++iter)
		{
			if (iter->second == link)
			{
				links_.erase(iter); 
				return true;
			}
		}

		return false; // not found
	}

	void unlink_all(Key key)
	{
		links_.erase(key);
	}


private: 
	using map = std::multimap<Key, Link>;

	map links_;
};

} // actor
} // lax
