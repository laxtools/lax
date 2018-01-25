#pragma once 

#include <lax/net/protocol/util/modifier.hpp>
#include <mutex>

namespace lax
{
namespace net
{

struct cipher_helper;

/// encryption / decryption modifier 
/**
 * feature: 
 * - use AES128 / CBC / PKCS7 
 * - nonce changes on every recv and send with sha1
 * - key changes on config::key_change_interval 
 */
class cipher final : public modifier
{
public:
	struct config
	{
		/// key 변경 간격. 송신이나 수신 개수 기준. 1024 기본값
		std::size_t key_change_interval = 1024; 
	};
	static config cfg;

	cipher(); 

	/// destructor. required to genereate cipher_helper destructor
	~cipher();

	virtual result on_bind() override;

	/// after recv. buf has other messages in buffer. 
	virtual result on_recv(
		resize_buffer& buf, 
		std::size_t msg_pos, 
		std::size_t msg_len, 
		std::size_t& new_len
	) override;

	/// before send. buf has only this message
	virtual result on_send( 
		resize_buffer& buf, 
		std::size_t msg_pos, 
		std::size_t msg_len
	) override;

private: 
	std::unique_ptr<cipher_helper> receiver_;
	std::unique_ptr<cipher_helper> sender_;

	std::recursive_mutex mutex_;
};

} // net
} // lax