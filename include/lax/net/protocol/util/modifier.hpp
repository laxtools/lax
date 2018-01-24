#pragma once 

#include <lax/net/reason.hpp>
#include <lax/net/detail/buffer/resize_buffer.hpp>
#include <lax/util/result.hpp>

namespace lax
{
namespace net
{

class protocol;

/// modifies and verifies messages
/** 
 * modifier is used with each protocol to provide: 
 * - sequence check 
 * - checksum check 
 * - cipher check
 * - and more...
 */
class modifier
{
public:
	using result = util::result<bool, reason>;

public:
	modifier() = default;

	virtual ~modifier() = default;

	result bind(protocol* proto)
	{
		expect(proto);
		return_if(!proto, result(false, reason::fail_null_pointer));

		protocol_ = proto;

		return on_bind();
	}

	virtual result on_bind() = 0;

	/// after recv. buf has other messages in buffer. 
	/** 
	 * usually it is not necessary to modify buffer. 
	 * - just read and verify spec
	 * - thread-safe to read and process buffer
	 */
	virtual result on_recv(
		resize_buffer& buf, 
		std::size_t msg_pos, 
		std::size_t msg_len
	) = 0;

	/// before send. buf has only this message
	/** 
	 * must provide thread-safey. called from multiple threads
	 *
	 * changes buffer
	 * - length field of message  
	 * - resize (padding, sequence, checksum field)
	 * - each field with encryption
	 */
	virtual result on_send(
		resize_buffer& buf, 
		std::size_t msg_pos, 
		std::size_t msg_len
	) = 0;

protected:
	protocol* get_protocol()
	{
		return protocol_;
	}

private: 
	protocol* protocol_ = nullptr;
};

} // net
} // lax