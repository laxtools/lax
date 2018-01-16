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

class service;

class acceptor final
{
public: 
	using ptr = std::shared_ptr<acceptor>;

	acceptor(service* svc, uint16_t id, service::creator& creator, const std::string& addr);

	~acceptor();

	/// 지정된 주소에서 accept 시작.
	service::result listen(); 

	/// session 아이디를 받아서 세션 생성. 
	session::ptr create(const session::id& id, tcp::socket&& soc);

	const addr& get_addr() const
	{
		return addr_;
	}

private: 
	void do_accept();
	void on_accepted(const asio::error_code& ec);

private: 
	service* svc_ = nullptr;
	uint16_t id_ = 0;
	service::creator creator_;
	addr addr_;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

} // net
} // lax
