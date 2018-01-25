#include "stdafx.h"
#include <lax/net/protocol/bits/bits_message_factory.hpp>
#include <lax/util/logger.hpp>
#include <lax/util/exception.hpp>

namespace lax
{
namespace net
{

bits_message_factory& bits_message_factory::inst()
{
	static bits_message_factory inst_;

	return inst_;
}

void bits_message_factory::add(const packet::topic_t& topic, creator c)
{
	auto iter = map_.find(topic);

	if (iter != map_.end())
	{
		util::log()->warn(
			"bits_message w/ topic[{}:{}] is alreay added!",
			topic.get_group(), topic.get_type());

		util::log()->flush();

		// THROW("bits_messag_factory. bits_message already added!");
	}

	return_if(iter != map_.end());

	map_[topic] = c;
}

bits_message::ptr  bits_message_factory::create(const packet::topic_t& topic) const
{
	auto iter = map_.find(topic);
	return_if(iter == map_.end(), bits_message::ptr());

	return iter->second();
}

} // net
} // lax