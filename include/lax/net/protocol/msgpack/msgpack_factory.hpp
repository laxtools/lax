#pragma once 

#include <lax/net/protocol/msgpack/msgpack_message.hpp>
#include <lax/net/reason.hpp>
#include <lax/util/result.hpp>
#include <map>

namespace lax
{
namespace net
{

/// msgpack binary format 
/**
 * creates a msgpack_message from category and type  
 * 
 * 정적으로 시스템 시작 시 구성하고 락을 사용하지 않음
 */
class msgpack_factory 
{
public: 
	using result = util::result<bool, reason>;

public: 
	static msgpack_factory& instance();

	/// m은 msgpack_message. domain과 type가 유효해야 함
	result add(msgpack_message::ptr m);

	/// domain, type static 변수를 갖는 msgpack_message 하위 클래스 등록
	template <typename Msg> 
	result add()
	{
		auto m = std::make_shared<Msg>();

		return add(m);
	}

	/// create from domain and type merged
	msgpack_message::ptr create(uint32_t domain_type);

	/// create from domain and type 
	msgpack_message::ptr create(uint16_t domain, uint16_t type)
	{
		return create(domain << 16 | type);
	}

private:
	using prototypes = std::map<uint32_t, msgpack_message::ptr>;

	msgpack_factory() = default;

	prototypes prototypes_;
};

} // net
} // lax

/// 팩토리 등록.
#define MSGPACK_REGISTER(CLS) msgpack_factory::instance().add<CLS>();