#pragma once 

#include <lax/net/session.hpp>
#include <lax/net/protocol.hpp>

namespace lax
{
namespace net
{

class protocol_creator
{
public: 
	using ptr = std::shared_ptr<protocol_creator>;

	virtual protocol::ptr create(session::ptr ss) = 0;
};

} // net 
} // lax
