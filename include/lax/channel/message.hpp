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
	message(const topic_t& topic = topic_t(0))
		: topic_(topic)
	{
	}

	virtual ~message()
	{
	}

	const topic_t& get_topic() const
	{
		return topic_;
	}

	bool is_valid_topic(const topic_t& topic) const
	{
		return topic.is_valid();
	}

	// for test 
	void set_topic(const topic_t& tp)
	{
		topic_ = tp;
	}

	virtual const char* get_desc() const
	{
		return "message";
	}

private: 
	topic_t topic_; // invalid
};

} // net
} // lax
