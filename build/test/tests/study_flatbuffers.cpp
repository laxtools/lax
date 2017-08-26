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
		// cmake (binary ����)
		// flatc �� ������ϸ� ������ ��
	}

	SECTION("tutorial")
	{
		// https://google.github.io/flatbuffers/flatbuffers_guide_tutorial.html

		using namespace MyGame::Sample;
		using namespace flatbuffers;

		FlatBufferBuilder builder(1024); // 1024���� ���� �ڵ����� ũ�� Ȯ��

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

		// ���̵��: Ʈ���� �����ϰ� ���� �ִ´�. �׷��� flatbuffer �̴�.
		// ���� Ʈ�� �����ʹ� offset���� �����Ѵ�. ���� ��尡 �� �ʿ� �´�. 

		// preorder, depth-first�� ��ü�� �迭 ���·� ����	

		builder.Finish(orc);

		// can be used for other use

		uint8_t *buf = builder.GetBufferPointer();
		auto size = builder.GetSize(); // current size

		auto monster = GetMonster(buf);

	}

	SECTION("��")
	{
		// �ڵ� ǰ���� ���� flat�ϰ� ����ٴ� ���̵� ���� 
		// �ڵ� ǰ���̳� ������ �ڵ嵵 ���� 
		// ��� �ϳ��� ��� �����ϴٴ� �͵� ����. 

		// ������ Ÿ�� ���� ��ȯ�� ���̺���� ���ø����̼ǿ��� 
		// ���� ������� ���ϰ� ������ ����� �ϰ� 
		// ���ۿ��� �о� ���� ������ ���� ��뼺 ������ �ִ�. 
	}

}