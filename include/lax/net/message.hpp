#pragma once 

#include <lax/channel/message.hpp>

#include <memory>
#include <string>

namespace lax
{
namespace net
{

class message : public channel::message
{
public:
	message()
	{
	}

	virtual ~message()
	{
	}

private:

};

} // net
} // lax
