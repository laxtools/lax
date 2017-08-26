#pragma once 

#include <lax/net/service.h>

namespace lax
{
namespace test
{

/// 테스트용 app 기반 클래스. 필요한 부분만 구현하기 위해 사용 
struct test_app : public lax::net::app
{
	/// called when session is ready 
	virtual void on_ready(const lax::net::session::id& id) override {}

	/// called when session is closed  
	virtual void on_error(const lax::net::session::id& id, const asio::error_code& ec) override {}

	/// called when session has a message to handle
	virtual void on_recv(const lax::net::session::id& id, lax::net::msg::ptr m) override {}

	/// called when accept failed
	virtual void on_accept_failed(const std::string& addr) override {}

	/// called when connect failed  
	virtual void on_connect_failed(const std::string& addr) override {}
};

} // test
} // lax
