#include "stdafx.h"
#include <lax/net/protocol/bits/bits_message_factory.hpp>

namespace lax
{
namespace net
{

bits_message_factory& inst()
{
	static bits_message_factory inst_;

	return inst_;
}

void bits_message_factory::add(const message::topic_t& topic, creator c)
{
	auto iter = map_.find(topic);

	check(iter == map_.end());
	return_if(iter != map_.end());

	map_[topic] = c;
}

bits_message::ptr  bits_message_factory::create(const message::topic_t& topic) const
{
	auto iter = map_.find(topic);
	return_if(iter == map_.end(), bits_message::ptr());

	return iter->second();
}

} // net
} // lax