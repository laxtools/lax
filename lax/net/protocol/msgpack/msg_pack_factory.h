#pragma once 

#include <lax/net/protocol/msgpack/msg_pack.h>
#include <map>

namespace lax
{
namespace net
{

/// msgpack binary format 
/**
 * creates a msg_pack from category and type  
 * 
 * 정적으로 시스템 시작 시 구성하고 락을 사용하지 않음
 */
class msg_pack_factory 
{
public: 
	using result = util::result<bool, reason>;

public: 
	static msg_pack_factory& instance();

	/// m은 msg_pack. domain과 type가 유효해야 함
	result add(msg_pack::ptr m);

	/// domain, type static 변수를 갖는 msg_pack 하위 클래스 등록
	template <typename Msg> 
	result add()
	{
		auto m = std::make_shared<Msg>();

		return add(m);
	}

	/// create from domain and type merged
	msg_pack::ptr create(uint32_t domain_type);

	/// create from domain and type 
	msg_pack::ptr create(uint16_t domain, uint16_t type)
	{
		return create(domain << 16 | type);
	}

private:
	using prototypes = std::map<uint32_t, msg_pack::ptr>;

	msg_pack_factory() = default;

	prototypes prototypes_;
};

} // net
} // lax

/// 팩토리 등록.
#define MSGPACK_REGISTER(CLS) msg_pack_factory::instance().add<CLS>();