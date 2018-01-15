#pragma once 

#include <lax/net/session.hpp>
#include <lax/net/proto.hpp>

namespace lax
{
namespace net
{

class proto_creator
{
public: 
	virtual proto::ptr create(session::ptr ss) = 0;
};

} // net 
} // lax
