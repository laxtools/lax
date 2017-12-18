#pragma once 

#include <lax/channel/sub.hpp>
#include <unordered_map>

namespace lax
{
namespace channel
{

/// a simple message dispatcher
/** 
 * �����ϰ� �ϳ��� Ŭ�������� ����ϴ� �뵵. 
 * - ���� �Լ��� ���.
 * - Ŭ������ ������ ���� ��.
 *
 * �׷� �������� ����
 */
class dispatcher
{
public: 
	dispatcher() = default;

	/// subscribe. unsubscribe�� ����
	bool subscribe(const message::topic_t& topic, sub::cb_t cb)
	{
		auto iter = subs_.find(topic);

		entry& e = entry();

		if (iter != subs_.end())
		{
			e = iter->second;
		}

		e.subs.push_back(
			sub(
				seq_++,	// for debug only
				topic,  // the topic
				[](message::ptr m) { return true;  },  // don't care
				cb,  // callback
				sub::mode::immediate // don't care
			)
		);

		subs_.emplace(topic, e);

		return true;
	}

	/// post.
	std::size_t post(message::ptr m)
	{
		return_if(!m->get_topic().is_valid(), 0);

		auto iter = subs_.find(m->get_topic());
		return_if(iter == subs_.end(), 0);

		int count = 0;

		auto& subs = iter->second.subs;

		for (auto& sub : subs)
		{
			if (sub.post(m))
			{
				++count;
			}
		}

		return count;
	}

private:
	struct entry
	{
		std::vector<sub>	subs;
	};
	using map = std::unordered_map<topic, entry>;

private:
	map subs_;
	uint32_t seq_ = 1;
};

} // channel
} // lax