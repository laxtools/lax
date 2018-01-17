#pragma once 

#include <lax/net/protocol/protocol.hpp>

namespace lax
{
namespace net
{

/** 
 *  
 * 
 * encryption: 
 * - AES CBC mode. PKCS7 
 * - key change with sha256 
 * - mersen engine for random data generation
 */
class msgpack_protocol : public protocol
{
public:
	struct config
	{
		// static on initialization
		bool enable_heartbeat = false;
		bool enable_crc32 = true;
		bool enable_encryption = true;				// AES CBC mode
		bool enable_encryption_key_change = true;	// changes encryption key
		int encryption_key_change_count = 1024;		// changes on messages sent or recvd
		float heartbeat_server_timeout	= 31;
		float heartbeat_client_interval = 17;

		// can be changed
		float log_stat_min_interval = 60*7;
	};

	static config config;

	msgpack_protocol();

	~msgpack_protocol();

	virtual result send(message::ptr m) override;

	virtual void on_recv(uint8_t* bytes, std::size_t len) override;

	virtual void on_send(std::size_t len) override;

	virtual void on_error() override;

private: 
	void log_stat();
};

} // net 
} // lax