#include "stdafx.h"

#include <catch.hpp>

#include <array>

#include "monster_generated.h"


TEST_CASE("flatbuffers study")
{
	// claims: 
	// - small and fast 
	// - type safe
	// - code generation
	// - single header include integration 
	// - lots to learn from it 


	SECTION("build")
	{
		// github
		// cmake (binary 제공)
		// flatc 와 헤더파일만 얻으면 됨
	}

	SECTION("tutorial")
	{
		// https://google.github.io/flatbuffers/flatbuffers_guide_tutorial.html

		using namespace MyGame::Sample;
		using namespace flatbuffers;

		FlatBufferBuilder builder(1024); // 1024에서 시작 자동으로 크기 확장

		auto weapon_one_name = builder.CreateString("Sword");
		short weapon_one_damage = 3;
		auto weapon_two_name = builder.CreateString("Axe");
		short weapon_two_damage = 5;

		// Use the `CreateWeapon` shortcut to create Weapons with all the fields set.
		auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
		auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

		// serialize the data tree using depth-first, pre-order traversal.

		// Serialize a name for our monster, called "Orc".
		auto name = builder.CreateString("Orc");

		// Create a `vector` representing the inventory of the Orc. Each number
		// could correspond to an item that can be claimed after he is slain.
		unsigned char treasure[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		auto inventory = builder.CreateVector(treasure, 10);

		auto orc = CreateMonster(builder, &Vec3(1.0f, 2.0f, 3.0f), 10, 10, name, inventory);

		// 아이디어: 트리를 평평하게 만들어서 넣는다. 그래서 flatbuffer 이다.
		// 하위 트리 데이터는 offset으로 참조한다. 상위 노드가 뒤 쪽에 온다. 

		// preorder, depth-first로 전체를 배열 형태로 만듦	

		builder.Finish(orc);

		// can be used for other use

		uint8_t *buf = builder.GetBufferPointer();
		auto size = builder.GetSize(); // current size

		auto monster = GetMonster(buf);

	}

	SECTION("평가")
	{
		// 코드 품질이 좋고 flat하게 만든다는 아이디어도 좋고 
		// 코드 품질이나 생성된 코드도 좋고 
		// 헤더 하나로 사용 가능하다는 것도 좋다. 

		// 하지만 타겟 언어로 변환된 테이블들을 애플리케이션에서 
		// 직접 사용하지 못하고 일일이 써줘야 하고 
		// 버퍼에서 읽어 오는 구조로 인해 사용성 문제가 있다. 
	}

}