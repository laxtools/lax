#pragma once 

#include <lax/net/session.h>
#include <lax/net/reason.h>

#include <functional>

namespace lax
{
namespace net
{

struct app
{
	/// called when session is ready 
	virtual void on_ready(const session::id& id) = 0;

	/// called when session has a message to handle
	virtual void on_recv(const session::id& id, msg::ptr m) = 0;

	/// called when session is closed or error happened
	virtual void on_error(const session::id& id, const asio::error_code& ec) = 0;

	/// called when accept failed
	virtual void on_accept_failed(const std::string& addr) = 0;

	/// called when connect failed  
	virtual void on_connect_failed(const std::string& addr) = 0;
};

class service_impl;

class service
{
public: 
	using creator = std::function< 
		session::ptr ( 
			service& svc,
			session::id id,
			tcp::socket&& soc,
			bool accepted
		)>;

	using result = util::result<bool, reason>;

public:
	explicit service(app* ap);
	~service();

	/// addr에서 listen. addr은 ip:port 형식. 
	result listen(const std::string& addr, creator& c);

	/// connect to a addr. addr은 ip:port 형식
	result connect(const std::string& addr, creator& c);

	/// send to a session
	result send(const session::id& id, uint8_t* data, std::size_t len);

	/// close a session 
	void close(const session::id& id);

	/// session에서 에러 발생 시 호출
	void error(const session::id& id);

	/// get a session to handle directly 
	session::ptr get(const session::id& id);

	/// access app
	app* get_app();

	/// internal use only
	service_impl* impl() { return impl_.get(); }

private: 
	std::unique_ptr<service_impl>	impl_;
};

} // net
} // lax
