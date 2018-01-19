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

class service;

class acceptor final
{
public: 
	using ptr = std::shared_ptr<acceptor>;

	acceptor(uint16_t id, const std::string& protocol, const std::string& addr);

	~acceptor();

	/// 지정된 주소에서 accept 시작.
	service::result listen(); 

	const addr& get_addr() const
	{
		return addr_;
	}

	const std::string& get_protocol() const
	{
		return protocol_;
	}

private: 
	void do_accept();

	void on_accepted(const asio::error_code& ec);

private: 
	uint16_t id_ = 0;
	std::string protocol_;
	addr addr_;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

} // net
} // lax
