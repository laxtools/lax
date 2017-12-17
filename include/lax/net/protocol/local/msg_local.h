#pragma once 

#include <lax/net/msg.h>

namespace lax
{
namespace net
{

/// Local �ۼ��ſ� �޽��� 
/**
 *
 */
class msg_local : public msg
{
public:
	static const uint32_t invalid_key = 0;
	static const uint16_t invalid_type = 0;
	static const uint16_t invalid_seq = 0;

public:
	msg_local();
	~msg_local();

	/// key ���� 
	uint32_t get_key() const
	{
		return get_type() << 16 | get_seq();
	}

	/// msg_pack�� Ÿ���� ���´�. ����ڰ� ���Ѵ�.
	virtual uint16_t get_type() const
	{
		return invalid_type;
	}

	/// ���� �κ�. �����. �α� � ���
	virtual uint16_t get_seq() const
	{
		return invalid_seq;
	}

	uint64_t src = 0;
	uint64_t dst = 0; 
};

} // net
} // lax

  // domain type ���
#define MSGPACK_KEY(TYPE, SEQ) \
	uint16_t get_type() const override \
	{ \
		return (TYPE); \
	} \
	uint16_t get_seq() const override \
	{ \
		return (SEQ); \
	} 



