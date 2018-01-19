#pragma once 

#include <lax/channel/message.hpp>

#include <memory>
#include <string>

namespace lax
{
namespace net
{

struct message : public channel::message
{
	using sid = uint32_t;

	sid src = 0;	/// source session id

	message(const topic_t& topic)
		: channel::message(topic)
	{
	}
};

} // net
} // lax
