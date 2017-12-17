#pragma once 

#include <memory>
#include <string>

namespace lax
{
namespace channel
{

class message
{
public:
	using ptr = std::shared_ptr<message>;
	using proto_t = uint8_t;
	using topic_t = uint32_t;
	using channel_key_t = std::string;

public:
	message()
	{
	}

	virtual ~message()
	{
	}

	topic_t set_topic(proto_t topic)
	{
		topic_ = topic;
		return topic_;
	}

	topic_t get_topic() const
	{
		return topic_;
	}

	bool is_valid_topic(topic_t topic) const
	{
		return topic != 0;
	}

private:
	topic_t topic_ = 0; // invalid
};

} // net
} // lax
