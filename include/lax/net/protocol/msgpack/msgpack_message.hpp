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
 * msgpack 기반 통신 프로토콜의 기반 메세지 
 * 
 * 하위 클래스 작성: 
 *
 *  public:
 *   MSGPACK_KEY(0, 33);
 *   MSGPACK_BODY_OVERRIDE(msg_echo);
 *	 MSGPACK_DEFINE(iv, fv);
 * 
 * 위 세 가지 매크로로 정의. 모두 public이어야 함
 *
 *  - MSGPACK_KEY은 factory 등록에 필요한 변수 준비 
 *  - MSGPACK_BODY_VOERRIDE()는 pack/unpack 준비 함수 등록 
 *  - MSGPACK_DEFINE.. 부분은 pack/unpack 실제 구현 함수 등록
 *
 * 메세지에 포함할 오브젝트들은 MSGPACK_DEFINE... 부분만 작성.
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

/// inline / template 구현

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

// msgpack_message 자식 클래스에서 필요한 함수들 등록
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