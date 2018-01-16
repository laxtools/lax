#pragma once 

#include <lax/net/message.hpp>
#include <lax/net/reason.hpp>
#include <lax/net/detail/buffer/send_buffer.hpp>
#include <lax/util/result.hpp>

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
class msgpack_message : public message
{
public: 
	using result = util::result<bool, reason>;			
	using buffer = send_buffer<1024*64>;				/// 64K send block
	using ptr = std::shared_ptr<msgpack_message>;		

public: 
	msgpack_message();
	~msgpack_message();

	/// create a message 
	virtual msgpack_message::ptr create() = 0;

	/// pack this to a buffer. empty
	virtual result pack(buffer& buf) = 0;

	/// unpack this from data. empty
	virtual result unpack(const char* data, std::size_t len) = 0;

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
};

/// inline / template ����

template <typename Msg>
msgpack_message::result msgpack_message::pack(const Msg& m, buffer& buf)
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
msgpack_message::result msgpack_message::unpack(Msg& m, const char* data, std::size_t len)
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
		obj.convert(m);  
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

// msgpack_message �ڽ� Ŭ�������� �ʿ��� �Լ��� ���
#define MSGPACK_BODY_OVERRIDE(CLS)  \
	virtual msgpack_message::ptr create() override \
	{ \
		return std::make_shared<CLS>(); \
	} \
	\
	virtual result pack(buffer& buf) override \
	{ \
		return msgpack_message::pack(*this, buf); \
	} \
	\
	virtual result unpack(const char* data, std::size_t len) override \
	{ \
		return msgpack_message::unpack(*this, data, len); \
	}