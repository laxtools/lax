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

TEST_CASE("msgpack ����")
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
			//  --> msgpack::type::make_define_array(__VA_ARGS__)�� ���� �� ���� �Լ��� ���� 
			// 
			// define_array<> make_define_array();
			// template <typename... Args>
			// inline define_array<Args...> make_define_array(Args&... args);

			// define_array<Arg, Size> ���·� ��Ī�ؼ� ��ͷ� ó��
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

	// SECTION("sbuffer��� Ǯ�� �Ǵ� ���� ���")
	{
		// recv �� ���� segment_buffer ����ϸ� ����
		// sbuffer�� ����ϰ� ������ ���.
	}

	// SECTION("length �ʿ�")
	{
		// ���ٸ� �����ڰ� ���� ������ �ʿ�. pack �ϰ� ���� ���̸� �˾ƾ� ��

		// virtual �Լ��� ���
		// msg_pack::pack( segment_buffer& buf ); 
		// msg_pack::unpack( segment_buffer& buf );
	}

	// SECTION("type ������")
	{
		// msg_pack�� ��ü���� �ν��Ͻ� ����� 
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

TEST_CASE("msgpack ����")
{
	// build ���� 
	// �ٸ� include ������ �������� msg_pack.h �� msgpack.hpp �����ϸ� 
	// has_as<T>�� check(U*) �Լ����� void �����ͷ� �ؼ��� ������ ���� ������ ���� �߻� 

	// ���� �޼��� : 
	//		msgpack\include\msgpack\v1\object_fwd.hpp(63): error C2062: ����ġ ���� 'void' �����Դϴ�.

	// �ϴ� ��ȸ�ϱ� ���� stdafx.h�� �����ؼ� ó��

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

	// template ����̶� Ÿ���� �˰� ���� �� serialize / deserialize �ϴ� �� ���� ����. 
	// �̸� ���� �����ϵ��� �Ѵ�. 

	// category / type�� base���� ���� 
	// �� �� ���θ� �鿩�� ���� �۾��Ѵ�. ���� ������ ������ �� ����. 

	msg_design md;
	md.v1 = 3;
	md.f1 = 99.3f;

	msgpack::sbuffer buf;
	md.pack(buf);

	msg_design md2; 
	md2.unpack(buf);

	REQUIRE(md2.v1 == md.v1);
	REQUIRE(md2.f1 == md.f1);

	// virtual ����� ���� Ÿ�� ���� 
	// pack / unpack �Լ��� ��ũ�η� ����  


	// unpack �� �� category / type�� �̸� ���� 
	// factory���� ������ �Ŀ� �����ؼ� unpack �Ѵ�.

	// pack �� ���� message�� ������ �� �� �ִ�. 
	// pack�� �ѹ��� �ϴ� �� �����Ƿ� �������� ���� ������ ���� ������
	// session_pack�� ���� �Լ��� �־ �ǰڴ�.  
}
