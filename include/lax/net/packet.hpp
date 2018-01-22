#pragma once 

#include <lax/channel/message.hpp>

#include <memory>
#include <string>

namespace lax
{
namespace net
{

struct packet : public channel::message
{
	using sid_t = uint32_t;

	sid_t id = 0;	/// session id

	packet(const topic_t::key_t key)
		: packet(topic_t(key))
	{
	}

	packet(const topic_t& topic)
		: channel::message(topic)
	{
	}

	const char* get_desc() const override
	{
		return "packet";
	}
};

} // net
} // lax

#define MSG_TOPIC(group, type) 	\
\
static constexpr lax::channel::topic::key_t  topic_key = \
(((group) << 16) & 0xFFFF0000) | ((type) & 0x0000FFFF); \
\
static const lax::channel::message::topic_t get_topic() \
{ return lax::channel::message::topic_t(topic_key); }


#define	MSG_CLASS(cls) \
cls() \
: lax::net::packet(topic_key) \
{ \
} \
\
const char* get_desc() const override \
{ \
	return #cls; \
}
