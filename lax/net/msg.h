#pragma once 

#include <memory>
#include <string>

namespace lax
{
namespace net
{

class msg
{
public:
	using ptr = std::shared_ptr<msg>;

public:
	explicit msg(int protocol)
		: protocol_(protocol)
	{
	}

	virtual ~msg()
	{
	}

	int get_protocol() const
	{
		return protocol_;
	}

	void set_session(uint32_t id)
	{
		session_ = id;
	}

	uint32_t get_session() const
	{
		return session_;
	}

private:
	int		 protocol_ = 0; // invalid
	uint32_t session_ = 0;
};

} // net
} // lax
