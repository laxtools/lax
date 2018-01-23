#pragma once 

#include <lax/net/protocol/protocol.hpp>
#include <lax/net/detail/buffer/resize_buffer.hpp>

namespace lax
{
namespace net
{

/// bitsery serialization protocol 
/** 
 * - length: 4 bytes
 * - encryption per message 
 * - crc32 per message
 */
class bits_protocol final : public protocol
{
public:
	/// static configuration for bits protocol
	struct config
	{
		bool enable_encryption = true;
		bool enable_crc32 = true;
		bool enable_detail_log = true;
	};

	static config cfg;

public:
	/// constructor
	bits_protocol(); 

	/// ensure session is closed
	~bits_protocol();

	/// send to a session after processing packet
	virtual result send(packet::ptr m) override;

private:
	/// session is bound
	virtual void on_bind() override;

	/// session calls this when received data
	virtual result on_recv(uint8_t* bytes, std::size_t len) override;

	/// session calls this when sent data 
	virtual void on_send(std::size_t len) override;

	/// session calls this when error ocurrs
	virtual void on_error(const asio::error_code& ec) override;

private: 
	resize_buffer recv_buf_;
};

} // net
} // lax
