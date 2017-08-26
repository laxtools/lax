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
 * �������� �ý��� ���� �� �����ϰ� ���� ������� ����
 */
class msg_pack_factory 
{
public: 
	using result = util::result<bool, reason>;

public: 
	static msg_pack_factory& instance();

	/// m�� msg_pack. domain�� type�� ��ȿ�ؾ� ��
	result add(msg_pack::ptr m);

	/// domain, type static ������ ���� msg_pack ���� Ŭ���� ���
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

/// ���丮 ���.
#define MSGPACK_REGISTER(CLS) msg_pack_factory::instance().add<CLS>();