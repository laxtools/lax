#pragma once 

#include <lax/net/message.hpp>
#include <lax/net/session.hpp>
#include <lax/util/result.hpp>

namespace lax
{
namespace net
{

/// abstract base class for protocols
class protocol
{
public: 
	using ptr = std::shared_ptr<protocol>;
	using result = util::result<bool, reason>;

	friend class session; // to use on_*

public:
	/// constructor
	protocol(session::ptr ss);

	/// destructor
	virtual ~protocol();

	/// used to log remote:sid:socket
	virtual const std::string& get_desc() const;

	/// send to a session after processing message
	virtual result send(message::ptr m) = 0; 

private:
	virtual void on_accepted() = 0;

	virtual void on_connected() = 0;

	/// session calls this when received data
	virtual void on_recv(uint8_t* bytes, std::size_t len) = 0;

	/// session calls this when sent data 
	virtual void on_send(std::size_t len) = 0;

	/// session calls this when error ocurrs
	virtual void on_error() = 0;

private:
	session::ptr			session_; /// session
};

} // net 
} // lax
