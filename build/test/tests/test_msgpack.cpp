#include "stdafx.h"

#include <msgpack.hpp>
#include <catch.hpp>

namespace
{

struct simple_msg
{
	int v;
	std::string sv;

	MSGPACK_DEFINE(v, sv);
};

}

TEST_CASE("msgpack 이해")
{
	SECTION("pack / unpack")
	{
		SECTION("using a single object")
		{
			simple_msg m;

			m.v = 33;
			m.sv = "hello";

			// serialize
			msgpack::sbuffer sbuf;
			msgpack::pack(sbuf, m);

			// deserialize
			msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

			// get object
			simple_msg m2;

			msgpack::object obj = oh.get();
			obj.convert(m2);

			REQUIRE(m2.v == m.v);
			REQUIRE(m2.sv == m.sv);
		}

		SECTION("msgpack_define")
		{
			// code generation by argument matching, type matching

			// MSGPACK_DEFINE 
			//	--> MSGPACK_DEFINE_ARRAY
			//  --> msgpack::type::make_define_array(__VA_ARGS__)을 갖는 세 개의 함수를 정의 
			// 
			// define_array<> make_define_array();
			// template <typename... Args>
			// inline define_array<Args...> make_define_array(Args&... args);

			// define_array<Arg, Size> 형태로 매칭해서 재귀로 처리
		}
	}
}

TEST_CASE("msg_pack design")
{
	// length and type required

	simple_msg m;

	m.v = 33;
	m.sv = "hello";

	// serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, m);

	// deserialize
	msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

	// get object
	simple_msg m2;

	msgpack::object obj = oh.get();
	obj.convert(m2);

	REQUIRE(m2.v == m.v);
	REQUIRE(m2.sv == m.sv);

	// SECTION("sbuffer대신 풀링 되는 버퍼 사용")
	{
		// recv 할 때도 segment_buffer 사용하면 좋음
		// sbuffer를 사용하고 별도로 고려.
	}

	// SECTION("length 필요")
	{
		// 별다른 구분자가 없기 때문에 필요. pack 하고 나서 길이를 알아야 함

		// virtual 함수를 사용
		// msg_pack::pack( segment_buffer& buf ); 
		// msg_pack::unpack( segment_buffer& buf );
	}

	// SECTION("type 구분자")
	{
		// msg_pack의 구체적인 인스턴스 만들기 
		// auto mp = msg_pack_factory::create( type );
		// mp->pack( buf, len ); 
		// mp->unpack( buf, len );
	}
}

#include <lax/net/protocol/msgpack/msg_pack.h>

using namespace lax::net;

namespace
{

struct msg_base
{
	uint16_t category = 0;
	uint16_t type = 0;

	virtual void pack(msgpack::sbuffer& buf)
	{

	}

	virtual void unpack(msgpack::sbuffer& buf)
	{

	}

	MSGPACK_DEFINE(category, type);
};

struct msg_design : public msg_base
{
	int v1 = 0;
	float f1 = 0;

	void pack(msgpack::sbuffer& buf) override
	{
		msgpack::pack(buf, *this);
	}

	void unpack(msgpack::sbuffer& buf) override
	{
		// deserialize
		msgpack::object_handle oh = msgpack::unpack(buf.data(), buf.size());

		msgpack::object obj = oh.get();
		obj.convert(*this);
	}

	MSGPACK_DEFINE(MSGPACK_BASE(msg_base), v1, f1);
};

}

TEST_CASE("msgpack 구현")
{
	// build 오류 
	// 다른 include 없으면 괜찮은데 msg_pack.h 에 msgpack.hpp 포함하면 
	// has_as<T>의 check(U*) 함수에서 void 포인터로 해석된 뭔가에 의해 컴파일 에러 발생 

	// 에러 메세지 : 
	//		msgpack\include\msgpack\v1\object_fwd.hpp(63): error C2062: 예기치 않은 'void' 형식입니다.

	// 일단 우회하기 위해 stdafx.h에 포함해서 처리

	/*

	* len(16) | category(16) | type(16) | object
	*
	* usage:
	*	msg_pack_factory::instance().register<class>(); or MSG_PACK_REGISTER(class)
	*
	* class msg_ntf_skill_cast : pubilc msg_pack
	* {
	*	MSGPACK_DEFINE(field1, field2, ...)
	* } 
	*/

	// template 기반이라 타잎을 알고 있을 때 serialize / deserialize 하는 게 가장 좋다. 
	// 미리 만들어서 전송하도록 한다. 

	// category / type은 base에서 전송 
	// 좀 더 내부를 들여다 보고 작업한다. 내일 정도면 가능할 것 같다. 

	msg_design md;
	md.v1 = 3;
	md.f1 = 99.3f;

	msgpack::sbuffer buf;
	md.pack(buf);

	msg_design md2; 
	md2.unpack(buf);

	REQUIRE(md2.v1 == md.v1);
	REQUIRE(md2.f1 == md.f1);

	// virtual 기반의 정적 타잎 결정 
	// pack / unpack 함수를 매크로로 정의  


	// unpack 할 때 category / type을 미리 보고 
	// factory에서 생성한 후에 포함해서 unpack 한다.

	// pack 할 때는 message만 있으면 할 수 있다. 
	// pack은 한번만 하는 게 좋으므로 상위에서 만들어서 보내는 것이 좋지만
	// session_pack에 편의 함수로 있어도 되겠다.  
}
