#pragma once 

#include <lax/util/macros.hpp>
#include <memory>

namespace lax
{
namespace channel
{

/// 메세지 전파에 사용하는 토픽 
/** 
 * group | type 으로 나누어서 처리하는 것을 명시적으로 요구. 
 * 
 * 유연하면 복잡하고 단순하면 기능이 적다. 
 * 적절한 균형점이 type / seq posting으로 정하고 간다.
 */
class topic
{
public: 
	using key_t = uint32_t;
	using group_t = uint16_t;
	using type_t = uint16_t;

public: 
	topic()
		: key_(0)
	{
	}

	explicit topic(key_t key)
		: key_(key)
	{
	}

	explicit topic(group_t group, type_t type)
		: key_(group << 16 | type)
	{
	}

	topic(const topic& rhs)
	{
		*this = rhs;
	}

	topic& operator = (const topic& rhs)
	{
		key_ = rhs.key_;

		return *this;
	}

	key_t get_key() const
	{
		return key_;
	}

	group_t get_group() const
	{
		return (key_ >> 16) & 0x0000FFFF;
	}

	type_t get_type() const
	{
		return key_ & 0x0000FFFF;
	}

	topic get_group_topic() const
	{
		return topic(get_group(), 0);
	}

	bool is_valid() const
	{
		return key_ > 0;
	}

	bool match(const topic& other)
	{
		if (get_group() == other.get_group())
		{
			if (get_type() == 0)
			{
				return true;
			}

			return get_type() == other.get_type();
		}

		return false;
	}

	topic& operator=(key_t key)
	{
		key_ = key;

		return *this;
	}

	bool operator == (const topic& rhs) const
	{
		return key_ == rhs.key_;
	}

	bool operator != (const topic& rhs) const
	{
		return !(operator == (rhs));
	}

	bool operator < (const topic& rhs) const
	{
		return key_ < rhs.key_;
	}

	bool operator > (const topic& rhs) const
	{
		return key_ > rhs.key_;
	}

private: 
	key_t key_;
};

} // channel
} // lax

// TODO: 
//  - add hash function for topic
#include <unordered_map>

namespace std {

template <>
struct hash<::lax::channel::topic>
{
	std::size_t operator()(const ::lax::channel::topic& k) const
	{
		auto key = static_cast<uint32_t>(k.get_key());

		return std::hash<uint32_t>()(key);
	}
};

} // std
