#pragma once 
#include <typeinfo>

namespace lax
{
namespace actor
{

class type_info
{
public: 
	static type_info no_type;

	type_info()
		: type_info(no_type)
	{
	}

	type_info(const std::type_info& info)
		: info_(info)
	{
	}

	std::size_t hash_code() const 
	{
		return info_.hash_code();
	}

	bool operator==(const type_info& other) const 
	{
		return info_ == other.info_;
	}

	bool operator!=(const type_info& other) const 
	{
		return info_ != other.info_;
	}

	const char* name() const 
	{
		return info_.name();
	}

private: 
	const std::type_info& info_;
};

} // actor
} // lax

namespace std
{

/// hash function
template <>
struct hash<::lax::actor::type_info>
{
	std::size_t operator()(const ::lax::actor::type_info& obj) const
	{
		return obj.hash_code();
	}
};

} // std