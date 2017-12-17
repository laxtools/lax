#pragma once 

#include <lax/net/msg.h>
#include <lax/net/reason.h>
#include <lax/net/detail/segment.h>
#include <lax/util/result.h>

namespace lax
{
namespace net
{

/// msgpack binary format 
/**
 * msgpack ��� ��� ���������� ��� �޼��� 
 * 
 * ���� Ŭ���� �ۼ�: 
 *
 *  public:
 *   MSGPACK_KEY(0, 33);
 *   MSGPACK_BODY_OVERRIDE(msg_echo);
 *	 MSGPACK_DEFINE(iv, fv);
 * 
 * �� �� ���� ��ũ�η� ����. ��� public�̾�� ��
 *
 *  - MSGPACK_KEY�� factory ��Ͽ� �ʿ��� ���� �غ� 
 *  - MSGPACK_BODY_VOERRIDE()�� pack/unpack �غ� �Լ� ��� 
 *  - MSGPACK_DEFINE.. �κ��� pack/unpack ���� ���� �Լ� ���
 *
 * �޼����� ������ ������Ʈ���� MSGPACK_DEFINE... �κи� �ۼ�.
 */
class msg_pack : public msg
{
public:
	static const uint32_t invalid_key = 0;
	static const uint16_t invalid_type = 0;
	static const uint16_t invalid_seq = 0;

public: 
	using result = util::result<bool, reason>;
	using buffer = msgpack::sbuffer;
	using ptr = std::shared_ptr<msg_pack>;

public: 
	msg_pack();
	~msg_pack();

	/// create a message 
	virtual msg_pack::ptr create() = 0;

	/// pack this to a buffer. empty
	virtual result pack(buffer& buf) = 0;

	/// unpack this from data. empty
	virtual result unpack(const char* data, std::size_t len) = 0;

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

	/// utility template to implement pack
	/** 
	 * pack(buf) 
	 * { 
	 *   pack(*this, buf );
	 * }
	 */
	template <typename Msg> 
	result pack(const Msg& m, buffer& buf);

	/// utility template to implement unpack
	/** 
	 * unpack(buf) 
	 * { 
	 *   unpack( *this, buf );
	 * }
	 */
	template <typename Msg> 
	result unpack(Msg& m, const char* data, std::size_t len);

	/// check validity
	bool is_valid() const
	{
		return valid_;
	}

protected: 
	const uint32_t	key_	= 0;
	bool			valid_	= false;
};

/// inline / template ����

template <typename Msg>
msg_pack::result msg_pack::pack(const Msg& m, buffer& buf)
{
	valid_ = false;

	try
	{
		msgpack::pack(buf, m);
	}
	catch (std::exception& ex)
	{
		ex;
		return result(false, reason::fail_msg_pack_serialize);
	}

	valid_ = true;

	return result(true, reason::success);
}

template <typename Msg>
msg_pack::result msg_pack::unpack(Msg& m, const char* data, std::size_t len)
{
	valid_ = false;

	check(data);
	check(len > 0);

	return_if(
		data == nullptr,
		result(false, reason::fail_null_pointer)
	);

	return_if(
		len == 0,
		result(false, reason::fail_zero_size_data)
	);

	try
	{
		msgpack::object_handle oh = msgpack::unpack(data, len);

		msgpack::object obj = oh.get();
		obj.convert(m);  // ���� üũ��?
	}
	catch (std::exception& ex)
	{
		ex;
		return result(false, reason::fail_msg_pack_deserialize);
	}

	valid_ = true;

	return result(true, reason::success);
}

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

// msg_pack �ڽ� Ŭ�������� �ʿ��� �Լ��� ���
#define MSGPACK_BODY_OVERRIDE(CLS)  \
	virtual msg_pack::ptr create() override \
	{ \
		return std::make_shared<CLS>(); \
	} \
	\
	virtual result pack(buffer& buf) override \
	{ \
		return msg_pack::pack(*this, buf); \
	} \
	\
	virtual result unpack(const char* data, std::size_t len) override \
	{ \
		return msg_pack::unpack(*this, data, len); \
	}