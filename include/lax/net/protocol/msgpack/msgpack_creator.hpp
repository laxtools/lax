#pragma once 

#include <lax/net/protocol_creator.hpp>

namespace lax
{
namespace net
{

class msgpack_creator : public protocol_creator
{
public:
	msgpack_creator();

	~msgpack_creator();


private: 

};

} // net 
} // lax