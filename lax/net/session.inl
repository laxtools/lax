#pragma once 

#include "session.h"

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

} // net 
} // lax


