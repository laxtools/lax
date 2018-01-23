#pragma once 

#include <lax/net/protocol/protocol.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
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
		bool enable_cipher = true;

		bool enable_checksum = true;
	
		bool enable_detail_log = false;
		/// 테스트를 위해 send에서 바로 on_recv를 호출
		bool enable_loopback = false;	
	};

	static config cfg;

public:
	/// constructor
	bits_protocol(); 

	/// ensure session is closed
	~bits_protocol();

	/// send to a session after processing packet
	virtual result send(packet::ptr m) override;

	/// 미리 serialize 해서 요청을 보낼 필요. --> 멀티캐스팅
	/** 
	 * 멀티캐스트 같은 경우 미리 시리얼라이즈 해서 전송
	 * packet도 포함하도록 해서 그냥 바이트 보내지 않도록 한다.  
	 */
	virtual result send(
		packet::ptr m, 
		const uint8_t* const data, 
		std::size_t len
	);

private:
	/// cipher, checksum, then send to session
	result send_final(
		bits_message::ptr mp, 
		const uint8_t* const data, 
		std::size_t len
	);

	/// session is bound
	virtual void on_bind() override;

	/// session calls this when received data
	virtual result on_recv(
		const uint8_t* const bytes, 
		std::size_t len
	) override;

	/// session calls this when sent data 
	virtual void on_send(std::size_t len) override;

	/// session calls this when error ocurrs
	virtual void on_error(const asio::error_code& ec) override;

	uint32_t get_length(resize_buffer::iterator& iter) const; 

	uint32_t get_topic(resize_buffer::iterator& iter) const; 


public:
	/// for test only 
	result call_recv_for_test(const uint8_t* const bytes, std::size_t len);     

private: 
	resize_buffer recv_buf_;
};

} // net
} // lax
