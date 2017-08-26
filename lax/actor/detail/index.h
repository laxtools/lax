#pragma once 

#include <lax/util/macros.h>
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
 * Key, Link  : 값으로 처리
 *
 * NOTE: Link의 복사 성능 주의
 */
template <class Key, class Link>
class index
{
public:
	using result = util::result<bool, Link>;

	index() = default;
	~index() = default;

	bool link(Key key, Link link)
	{
		auto rc = links_.insert(map::value_type(key, link));

		return rc.second;
	}

	result find(Key key) const
	{
		auto iter = links_.find(key);
		return_if(iter == links_.end(), result(false, null_));

		return result(true, iter->second);
	}

	bool has(Key key) const
	{
		return (bool)find(key);
	}

	void unlink(Key key)
	{
		links_.erase(key);
	}

private: 
	using map = std::unordered_map<Key, Link>;

	map links_;
	Link null_;
};

} // actor
} // lax
