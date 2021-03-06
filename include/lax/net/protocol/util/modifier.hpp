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
 * 
 * TODO: interface가 복잡해져서 context 개념을 추가한다. 
 * - context에 대해서만 락을 건다 
 * - context에 정보를 변경한다.
 * - 아직은 불확실하다. 
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
		EXPECT(proto);
		RETURN_IF(!proto, result(false, reason::fail_null_pointer));

		protocol_ = proto;

		auto rc = on_bind();
		bound_ = !!rc;

		return rc;
	}

	virtual result on_bind() = 0;

	/// after recv. buf has other messages in buffer. 
	/** 
	 * usually it is not necessary to modify buffer. 
	 * - just read and verify spec
	 * - thread-safe to read and process buffer
	 * 
	 * @param new_len - new message length after processing
	 */
	virtual result on_recv(
		resize_buffer& buf, 
		std::size_t msg_pos, 
		std::size_t msg_len, 
		std::size_t& new_len			
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

	/// update length field to reflect change
	void update_length_field(
		resize_buffer& buf, 
		std::size_t msg_pos, 
		std::size_t new_len
	);

protected: 
	std::atomic<bool> bound_ = false;

private: 
	protocol* protocol_ = nullptr;
};

} // net
} // lax