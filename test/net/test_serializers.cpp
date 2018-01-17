#include "stdafx.h"
#include <catch.hpp>
#include <flatbuffers/flatbuffers.h>
#include <lax/util/simple_timer.hpp>
#include <iostream>

#include "monster_generated.h"

TEST_CASE("serializers")
{
	//
	// msgpack�� �������� ������.
	// thrift binary ���� ������. 
	// flatbuffers, yas, capnproto �� �� ������ �ִ� 
	// �ʴ� �鸸 �� �̻��� �����ؾ� �ϴµ� ���� �ʴ�. 
	// �ϴ� flatbuffers�� decode���� 2�鸸 �̻�, ���ڵ��� 30�� �̻��� �����Ѵ�. 
	// ���� ���� type-safe, ��Ƽ�÷���, ��Ƽ��� ���� ������ �� ����. 
	// ������ MSGPACK_DEFINE()ó�� ������ �ʴ�. 
	// �ٽ� �ѹ� �����Ѵ�.
	


	SECTION("capnproto")
	{
		// �� �� ���Ѻ��� �Ѵ�.
		// �ϴ�, �ڵ尡 �����ϰ� ���� ����. 
		// �÷��� ������ ���������� �˱� ��ƴ�. 
		// flatbuffer�� ������ (8K+ likes)�� ���� �ִ�. 
		// msgpack-c ������ ���� �̽��� �ִ�. �ٲ� �� ������??
	}

	SECTION("flatbuffers")
	{
		using namespace flatbuffers;

		// https://code.facebook.com/posts/872547912839369/improving-facebook-s-performance-on-android-with-flatbuffers/
		
		// offset�� ���� vtable�� ���� �����͸� ã�Ƽ� ����. 
		// �޸� �Ľ��� ���� ���ϴ� �����ͷ� ��ȯ��. 
		// �޸� �Ҵ�� �Ľ��� �߰��� ���� ����
		// C++�� flatbuffers.h �ϳ��� ��. ����. 

		// GetMonster() �Լ��� ������ �Ҵ����� �ʰ� access �� ������ 
		// get_hp()�� �ϸ� hp�� ����Ű�� offset���� ���� ���� 
		// ���� �޼����� �޸� ���� �̵��ϰ� �� 

		SECTION("basic usage")
		{
			FlatBufferBuilder fbb;

			fb::Vec3 pos(1, 100, 10);

			// offset ����� �ϸ鼭 ����. ���� ������Ʈ ��ŷ�� ����. Finish�� ����� �� 
			// ������ �� �� ��Ȯ�ϰ� �˷��� ���õ��� �� �ʿ�. 

			auto offset = fb::CreateMonsterDirect(fbb, &pos, 300, 600, "hello monster");
			fbb.Finish(offset);

			auto* monster = fb::GetMonster(fbb.GetBufferPointer());

			REQUIRE(monster->mana() == 300);
			REQUIRE(monster->hp() == 600);
		}

		SECTION("basic performance")
		{
			constexpr int test_count = 1000000;

			lax::util::simple_timer timer;

			for (int i = 0; i < test_count; ++i)
			{
				FlatBufferBuilder fbb;

				fb::Vec3 pos(1, 100, 10);

				// offset ����� �ϸ鼭 ����. ���� ������Ʈ ��ŷ�� ����. Finish�� ����� �� 
				// ������ �� �� ��Ȯ�ϰ� �˷��� ���õ��� �� �ʿ�. 

				auto offset = fb::CreateMonsterDirect(fbb, &pos, 300, 600, "hello monster");
				fbb.Finish(offset);

				auto* monster = fb::GetMonster(fbb.GetBufferPointer());

				//REQUIRE(monster->mana() == 300);
				//REQUIRE(monster->hp() == 600);
			}

			std::cout << "flatbuffer: " << test_count << " elapsed: " << timer.elapsed() << std::endl;

			//
			// 0.357 ��.... ���� ������Ʈ�� �۱� �ϴٸ�.
			// ����� �������� 2�� ���� ���̰� ����. 
			// ������ ���� ������ �ϰ� �߰� �޸� ���簡 �Ͼ�� ������ �ӵ��� ���� ��. 
			// 
			// ���� �� �����ϴ� ���� ������ ��� �������� ������ ���� ���̴�. 
			// 
			//
		}
	}

	SECTION("yas")
	{
		// ������ �����ϰ� C++�� �����ؼ� ��ü �޼����� ó�� 
		// C++ Ŭ������ �޼��� ���� Ŭ������ ������ �ϰ� �������� �ʵ��� �Ѵ�. 
		// cereal�� �����ϳ� �� �۰� ������. �ֳ��ϸ� ������ ���� ���� �ۼ� ��İ� ���. 
		// �Լ��� ������ �����ϴ� ����� msgpack�� ����. ���� ���� ������ �� ���� ��. 

		// 
	}
}
