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
 * �������� �ý��� ���� �� �����ϰ� ���� ������� ����
 */
class msgpack_factory 
{
public: 
	using result = util::result<bool, reason>;

public: 
	static msgpack_factory& instance();

	/// m�� msgpack_message. domain�� type�� ��ȿ�ؾ� ��
	result add(msgpack_message::ptr m);

	/// domain, type static ������ ���� msgpack_message ���� Ŭ���� ���
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

/// ���丮 ���.
#define MSGPACK_REGISTER(CLS) msgpack_factory::instance().add<CLS>();