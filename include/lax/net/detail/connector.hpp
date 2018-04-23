#pragma once 

#include <lax/net/service.hpp>
#include <lax/net/addr.hpp>

#include <asio.hpp>
#include <memory>

using namespace asio::ip;

namespace lax
{
namespace net
{

class connector final
{
public:
	using ptr = std::shared_ptr<connector>;

	connector(uint16_t id, const std::string& protocol, const std::string& addr);

	~connector();

	service::result connect();

	const addr& get_addr() const
	{
		return addr_;
	}

	const std::string& get_protocol() const
	{
		return protocol_;
	}

private: 
	void on_connected(const asio::error_code& ec);

private: 
	uint16_t id_ = 0;
	std::string protocol_;
	addr addr_;
	tcp::socket socket_;
};

} // net
} // lax
