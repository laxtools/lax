#pragma once 

#include <lax/net/session.hpp>

namespace lax {
namespace net {

inline
session::id session::get_id() const
{
	return id_;
}

inline
service& session::get_svc()
{
	return svc_;
}

inline
session::id::id(uint32_t value = 0)
	: value_(value)
{
}

inline
session::id::id(uint16_t seq, uint16_t index)
	: seq_(seq)
	, index_(index)
{
	check(is_valid());
}

inline
bool session::id::is_valid() const
{
	return seq_ > 0 && index_ >= 0;
}

inline
const uint32_t session::id::get_value() const
{
	return value_;
}

inline
const uint16_t session::id::get_seq() const
{
	return seq_;
}

inline
const uint16_t session::id::get_index() const
{
	return index_;
}

inline
bool session::id::operator==(const id& rhs) const
{
	return value_ == rhs.value_;
}

inline
bool session::id::operator!=(const id& rhs) const
{
	return value_ != rhs.value_;
}

inline
bool session::id::operator < (const id& rhs) const
{
	return value_ < rhs.value_;
}

inline
session::ref(session::ptr ss)
	: session_(ss)
{
	check(session_);
}

inline
session::result session::ref::send(message::ptr m)
{
	return_if(!is_valid(), );

	return session->send(m);
}

inline
bool session::ref::is_valid() const
{
	return !!session && session->is_open();
}

inline
const std::string& get_remote_addr() const
{
	return session_->get_remote_addr();
}

inline
void session::ref::close()
{
	if (is_valid())
	{
		session_->close();
	}
}

} // net 
} // lax


