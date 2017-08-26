#include "stdafx.h"

#include <catch.hpp>
#include <memory>

namespace
{

struct entity 
{
};

struct comp
{
};

struct npc_movement : public comp
{
	explicit npc_movement()
	{
	}

	virtual void move_to(int pos)
	{
		pos_ = pos;
	}

	int pos_ = 0;
};


struct boss_movement : public npc_movement
{
	virtual void move_to(int pos)
	{
		pos_ = pos * 2;
	}
};

struct npc : public entity
{
	npc()
	{
		movement_ = std::make_unique<npc_movement>(this);
	}

	std::unique_ptr<npc_movement> movement_;
};

struct boss : public npc
{
	boss()
	{
		// override ptr
		movement_ = std::make_unique<boss_movement>(this);
	}
};

} // noname

TEST_CASE("entity/component")
{
	SECTION("move")
	{
		npc a1;

		a1.movement_->move_to(3);

		REQUIRE(a1.movement_->pos_ == 3);

		boss b1;

		b1.movement_->move_to(3); 
		REQUIRE(b1.movement_->pos_ == 6);

		// RAII scheme 
		// 생성자에서 필요한 것들 얻어 오기  
		// casting 하고 안 맞으면 exception 
		// 초기에 확인하고 실행 시에는 안전하다. 

		// 메세지 전송
		// 최후의 수단, 또는 매우 편리하지만 확정되지 않은 동작을 위해 사용
		// 이 부분은 잘 모르겠다.... 
		// owner->signal( id, params ) --> unicasting
		// owner->signal( name, params ) --> unicasting with a component name
		// owner->signal( params ) --> broadcasting

		// introspection 기능 필요 
		// signal과 같은 강력한 함수를 구현하려면 필요하다. 
		// 어떤 방법이 좋을까? 언리얼 ? 매크로? 
		//  
	}

}