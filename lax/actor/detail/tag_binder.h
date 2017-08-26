#pragma once 

#include <lax/actor/detail/ref.h>
#include <lax/net/msg.h>
#include <lax/net/protocol/protocol.h>
#include <lax/net/protocol/json/msg_json.h>
#include <lax/net/protocol/local/msg_local.h>
#include <lax/net/protocol/msgpack/msg_pack.h>

#include <functional>
#include <string>
#include <unordered_map>

namespace lax
{
namespace actor
{

class tag_binder 
{
public:
	/// sub / post는 아래 개념들을 사용하여 처리한다
	/**
	 * tag
	 * - protocol(8) ikey(32) skey(std::string)
	 *
	 * tag dispatching
	 * - <tag, target>
	 *
	 * target
	 * - function
	 * - optional field lookup function
	 *	 - <value, func>
	 *
	 * field lookup function
	 * - (tag, msg) -> value
	 */
	 /// key to dispatch
	 /**
	  * map과 unordered_map의 키로 사용 가능하다.
	  */
	struct tag
	{
		using proto_t = uint8_t;
		using v_key_t = uint32_t;
		using v_type_t = uint16_t;	// v_key_의 상위 16비트
		using s_key_t = std::string;

		enum key_type
		{
			invalid,
			value,
			value_type,
			string
		};

		tag() = default;

		void set(proto_t proto, v_key_t key)
		{
			check(key_type_ == invalid);
			check(proto_ == 0);
			check(v_key_ == 0);

			proto_ = proto;
			v_key_ = key;
			key_type_ = key_type::value;
		}

		void set(proto_t proto, v_type_t type)
		{
			check(key_type_ == invalid);
			check(proto_ == 0);
			check(v_key_ == 0);

			proto_ = proto;
			v_key_ = type << 16 | 0;
			key_type_ = key_type::value_type;
		}

		void set(proto_t proto, const s_key_t& key)
		{
			check(key_type_ == invalid);
			check(proto_ == 0);
			check(s_key_.empty());

			proto_ = proto;
			s_key_ = key;
			key_type_ = key_type::string;
		}

		bool operator == (const tag& rhs) const
		{
			return proto_ == rhs.proto_ &&
				key_type_ == rhs.key_type_ &&
				v_key_ == rhs.v_key_ &&
				s_key_ == rhs.s_key_;
		}

		bool operator < (const tag& rhs) const
		{
			if (key_type_ == key_type::value || key_type_ == key_type::value_type)
			{
				if (proto_ < rhs.proto_) { return true; }

				if (v_key_ < rhs.v_key_) { return true; }
			}

			if (key_type_ == key_type::string)
			{
				if (proto_ < rhs.proto_) { return true; }

				if (s_key_ < rhs.s_key_) { return true; }
			}

			return false;
		}

		proto_t		proto_ = 0;
		key_type	key_type_ = invalid;
		v_key_t		v_key_ = 0;
		v_type_t    v_type_ = 0;
		s_key_t		s_key_;
	};

	struct target
	{
		using value = uint64_t;
		using cond = std::function<bool(net::msg::ptr, value)>;
		using exec = std::function<void(net::msg::ptr)>;

		target() = default;

		void set_exec(exec& ex)
		{
			exec_ = ex;
		}

		void set_cond(cond& ac, value av)
		{
			cond_ = ac;
			value_ = av;
		}

		cond cond_;
		exec exec_;
		value value_ = 0;
		int id_ = 0;
	};
}; // post

} // actor
} // lax

// hash function for tag
namespace std
{
template <>
struct hash<lax::actor::post::tag>
{
	uint64_t operator() (const lax::actor::post::tag& tv) const
	{
		if (tv.key_type_ == lax::actor::post::tag::key_type::value)
		{
			// 31만큼만 unsigned 안에서 올릴 수 있다
			return std::hash<uint64_t>()(tv.proto_ << 31 | tv.v_key_);
		}

		// 프로토콜 부분 반영 안 해도 문자열 프로토콜만 중첩되므로 괜찮음
		return std::hash<std::string>()(tv.s_key_);
	}
};
} // std