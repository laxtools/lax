#pragma once 

#include <lax/channel/topic.hpp>
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
	using topic_t = topic;
	using channel_key_t = std::string;

public:
	message()
	{
	}

	virtual ~message()
	{
	}

	const topic_t& set_topic(const topic_t& topic)
	{
		topic_ = topic;
		return topic_;
	}

	const topic_t& get_topic() const
	{
		return topic_;
	}

	bool is_valid_topic(const topic_t& topic) const
	{
		return topic.is_valid();
	}

private:
	topic_t topic_; // invalid
};

} // net
} // lax
