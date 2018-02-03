#pragma once 

#include <lax/net/session.hpp>

namespace lax {
namespace net {

inline
const session::id& session::get_id() const
{
	return id_;
}

inline
session::id::id(uint32_t value)
	: value_(value)
{
}

inline
session::id::id(uint16_t seq, uint16_t index)
	: seq_(seq)
	, index_(index)
{
	VERIFY(is_valid());
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
session::ref::ref(session::ptr ss)
	: session_(ss)
{
	VERIFY(session_);
}

inline
session::ref::~ref()
{
	if (key_ > 0)
	{
		session_.reset();
		session::unsub_close(key_);
	}
}

inline
session::result session::ref::send(packet::ptr m)
{
	RETURN_IF(
		!is_valid(), 
		session::result(false, reason::fail_invalid_session)
	);

	return session_->send(m);
}

inline
bool session::ref::is_valid() const
{
	return !!session_ && session_->is_open();
}

inline
const std::string& session::ref::get_desc() const
{
	return session_->get_desc();
}

inline
bool session::ref::sub_close(cb_t cb)
{
	RETURN_IF(!session_, false);

	key_ = session::sub_close(
		session_->get_id().get_value(),
		cb
	);

	return key_ > 0;
}

inline
void session::ref::close()
{
	if (is_valid())
	{
		session_->close();
	}
}

inline
static bool operator == (const session::ref& r1, const session::ref& r2) 
{
	return r1.get_id() == r2.get_id();
}

inline
static bool operator != (const session::ref& r1, const session::ref& r2) 
{
	return !(operator == (r1, r2));
}

inline
static bool operator < (const session::ref& r1, const session::ref& r2) 
{
	return r1.get_id() < r2.get_id();
}

inline
static bool operator > (const session::ref& r1, const session::ref& r2) 
{
	return r1.get_id() > r2.get_id();
}

} // net 
} // lax

#include <unordered_map>

namespace std {

template <>
struct hash<::lax::net::session::ref>
{
	std::size_t operator()(const ::lax::net::session::ref& k) const
	{
		auto key = static_cast<uint32_t>(k.get_id());

		return std::hash<uint32_t>()(key);
	}
};

} // std


