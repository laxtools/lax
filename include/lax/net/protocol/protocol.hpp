#pragma once 

#include <lax/net/packet.hpp>
#include <lax/net/reason.hpp>
#include <lax/util/result.hpp>
#include <asio.hpp>

namespace lax
{
namespace net
{

/// forward decl
class session;
using wptr = session*;


/// abstract base class for protocols
class protocol
{
public: 
	using ptr = std::shared_ptr<protocol>;
	using result = util::result<bool, reason>;

	friend class session; // to use on_*

public:
	/// constructor
	protocol();

	/// destructor
	virtual ~protocol();

	/// bind a session after creation
	bool bind(wptr ss);

	/// idx:addr
	const std::string& get_desc() const
	{
		return desc_;
	}

	/// send to a session after processing message
	virtual result send(packet::ptr m) = 0; 

protected:
	/// bind to session
	virtual void on_bind() = 0; 

	/// session calls this when received data
	virtual result on_recv(uint8_t* bytes, std::size_t len) = 0;

	/// session calls this when sent data 
	virtual void on_send(std::size_t len) = 0;

	/// session calls this when error ocurrs
	virtual void on_error(const asio::error_code& ec) = 0;

protected:
	wptr& get_session() { return session_;  }

private:
	wptr			session_ = nullptr;
	std::string		desc_;
};

} // net 
} // lax
