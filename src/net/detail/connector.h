#pragma once 

#include <lax/net/service.h>
#include <lax/net/detail/addr.h>

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

	connector(service* svc, uint16_t id, service::creator& creator, const std::string& addr);

	~connector();

	service::result connect();

	/// session 아이디를 받아서 세션 생성. 
	session::ptr create(const session::id& id, tcp::socket&& soc);

	const addr& get_addr() const
	{
		return addr_;
	}

private: 
	void on_connected(const asio::error_code& ec);

private: 
	service* svc_ = nullptr;
	uint16_t id_ = 0;
	service::creator creator_;
	addr addr_;
	tcp::socket socket_;
};

} // net
} // lax
