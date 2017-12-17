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

	/// key 정의 
	uint32_t get_key() const
	{
		return get_type() << 16 | get_seq();
	}

	/// msg_pack은 타잎을 갖는다. 사용자가 정한다.
	virtual uint16_t get_type() const 
	{
		return invalid_type;
	}

	/// 하위 부분. 디버깅. 로깅 등에 사용
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

/// inline / template 구현

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
		obj.convert(m);  // 에러 체크는?
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

// domain type 등록
#define MSGPACK_KEY(TYPE, SEQ) \
	uint16_t get_type() const override \
	{ \
		return (TYPE); \
	} \
	uint16_t get_seq() const override \
	{ \
		return (SEQ); \
	} 

// msg_pack 자식 클래스에서 필요한 함수들 등록
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