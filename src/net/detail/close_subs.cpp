#include "stdafx.h"
#include <lax/net/detail/close_subs.hpp>
#include <lax/net/protocol/sys/sys_messages.hpp>
#include <algorithm>

namespace lax
{
namespace net
{

close_subs::close_subs()
	: key_seq_(1, UINT16_MAX)
{
}

close_subs::~close_subs()
{
	// static���� ����. ����, ������ �ʿ� ����
	// �ٸ� ��� ����� �ٽ� ����
}

close_subs::key_t close_subs::subscribe(sid id, cb_t cb)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	// TODO: �ݺ��Ǵ� ����. �˰���ȭ �� ����� ã��

	auto iter = subs_.find(id);

	if (iter != subs_.end())
	{
		auto key = key_seq_.next();

		iter->second.push_back(entry{ key, cb });

		index_[key] = id;

		return key;
	}
	else
	{
		auto key = key_seq_.next();

		vec vs;
		vs.push_back(entry{ key, cb });

		subs_.insert(map::value_type(id, vs));

		index_[key] = id;

		return key;
	}
}

void close_subs::unsubscribe(key_t key)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	auto iter = index_.find(key);
	return_if(iter == index_.end());

	auto si = subs_.find(iter->second);
	check(si != subs_.end());
	return_if(si == subs_.end());

	auto& v = si->second;

	v.erase(
		std::remove_if(
			v.begin(),
			v.end(),
			[key](const entry& e) { return e.key == key; }
		),
		v.end()
	);
}

std::size_t close_subs::post(sid id, const asio::error_code& ec)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	auto si = subs_.find(id);
	return_if(si == subs_.end(), 0);

	auto& v = si->second;

	auto mp = std::make_shared<sys_session_closed>();
	mp->id = id;
	mp->ec = ec;
	
	// Ȯ��: v�� ũ��. ��κ� 1���� ������ ���.

	auto count = v.size();

	for (auto& e : v)
	{
		e.cb(mp);
		index_.erase(e.key);
	}
	
	subs_.erase(si);

	return count;
}

} // channel
} // lax