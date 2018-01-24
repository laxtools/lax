#pragma once 

#include <lax/net/protocol/util/modifier.hpp>

namespace lax
{
namespace net
{

class cipher final : public modifier
{
public:
	virtual result on_bind() override;

	/// after recv. buf has other messages in buffer. 
	virtual result on_recv(
		resize_buffer& buf,
		std::size_t msg_pos,
		std::size_t msg_len
	) override;

	/// before send. buf has only this message
	virtual result on_send(
		resize_buffer& buf,
		std::size_t msg_pos,
		std::size_t msg_len
	) override;
};

} // net
} // lax