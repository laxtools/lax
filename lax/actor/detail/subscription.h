#pragma once

#include <lax/util/sequence.h>
#include <memory>
#include <unordered_map>
#include <vector>

namespace lax
{
namespace actor
{

class actor; 

/// used by actor to remember subscriptions and release
/** 
 * subscription은 대부분 소멸자에서 처리.
 *
 * 개별적으로 중간에 해제할 경우 변수로 기억하고 해제.
 */
class subscription
{
public:
	using ptr = std::weak_ptr<actor>;

	subscription();
	~subscription();

	/// remember subscription and return id
	int remember(ptr target, int id);

	/// release the subscription. return false if not found
	bool forget(int id);
	
private: 
	struct mem
	{
		ptr target_; 
		int id_;
		int mem_id_;
	};

	using  map = std::unordered_map<int, mem>;

private:
	void forget_all();

private:
	map mems_;
	util::sequence<uint16_t> seq_;
};

} // actor
} // lax
