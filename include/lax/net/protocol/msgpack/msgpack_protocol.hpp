#pragma once 

#include <lax/net/protocol.hpp>

namespace lax
{
namespace net
{

class protocol_msgpack : public protocol
{
public:
	protocol_msgpack();

	~protocol_msgpack();

	virtual result send(message::ptr m) override;

	virtual void on_recv(uint8_t* bytes, std::size_t len) override;

	virtual void on_send(std::size_t len) override;

	virtual void on_error() override;

private: 

};

} // net 
} // lax