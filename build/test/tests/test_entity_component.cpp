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
		// �����ڿ��� �ʿ��� �͵� ��� ����  
		// casting �ϰ� �� ������ exception 
		// �ʱ⿡ Ȯ���ϰ� ���� �ÿ��� �����ϴ�. 

		// �޼��� ����
		// ������ ����, �Ǵ� �ſ� �������� Ȯ������ ���� ������ ���� ���
		// �� �κ��� �� �𸣰ڴ�.... 
		// owner->signal( id, params ) --> unicasting
		// owner->signal( name, params ) --> unicasting with a component name
		// owner->signal( params ) --> broadcasting

		// introspection ��� �ʿ� 
		// signal�� ���� ������ �Լ��� �����Ϸ��� �ʿ��ϴ�. 
		// � ����� ������? �𸮾� ? ��ũ��? 
		//  
	}

}