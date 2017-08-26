#pragma once

#include <lax/actor/detail/tag_binder.h>
#include <lax/util/sequence.h>
#include <shared_mutex>

namespace lax
{
namespace actor
{

class actor;

class mailbox
{
public: 
	mailbox(actor* owner);
	virtual ~mailbox();

	/// subscribe pulling from this to target 
	/**
	* match tag, then tag_binder to ref
	*/
	int subscribe(tag_binder::tag tv, tag_binder::target& tgt);

	/// unsubscribe tag value from ref
	bool unsubscribe(int id);

	/// unsubscribe a list. return unsub count
	int unsubscribe(std::vector<int>& ids);

	/// dispatch message. returns tag_bindered count
	int post(net::msg::ptr m) const;

private:
	struct subs
	{
		using targets = std::vector<tag_binder::target>;
		using map = std::unordered_map<tag_binder::tag, targets>;
		using index = std::unordered_map<int, tag_binder::tag>;

		subs();

		int sub(tag_binder::tag& atag, tag_binder::target& to);

		int post(net::msg::ptr m) const;

		bool unsub(int id);

		/// msg_pack의 경우 type에 대해서만 전파
		int tag_binder_by_tag(tag_binder::tag& tv, net::msg::ptr m) const;

		/// key 전체로 tag를 만들고 전파 
		bool build_tag_with_key(tag_binder::tag& tv, net::msg::ptr m) const;

		/// msg_pack의 경우 타잎으로만 tag를 만들고 전파
		bool build_tag_with_type(tag_binder::tag& tv, net::msg::ptr m) const;

		map subs_;
		util::sequence<uint16_t> seq_;
		index index_;
	};

private:
	actor* owner_;
	subs subs_;
	mutable std::shared_timed_mutex mutex_;
};

} // actor 
} // lax
