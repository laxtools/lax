#include "stdafx.h"
#include <catch.hpp>
#include <bitsery/bitsery.h>

TEST_CASE("bitsery test")
{
	//
	// test_serializers �׽�Ʈ�� ���� bitsery�� msgpack�� ���� 3�� ����, 
	// flatbuffers�� ���ؼ� 2�� ���� ���� ������ Ȯ�� �Ǿ���. 
	// yas�� �����ƺ������� shared_buffer�� ���ο� �־� 
	// ��Ʈ��ũ ��� �� ���縦 ���� �� ����. bitsery�� �̸� ����ؼ� ����Ǿ��� 
	// ���۸� ������ �� �ִ� �����̴�. 
	// bitsery�� ���ۿ� ���� �ӵ��� ������ �ӵ��� �����ش�.
	// 
	// �ᱹ serialization ������ �޸� �Ҵ�/������ ���縦 �󸶳� ���� �� �ִ� ���� �߿��ϴ�. 
	// msgpack�� ��� parsing ���ϵ� �� �Ǵ� ���̴�. 
	// C++ Ŭ�� / ������ ���� ������ ������ ó���� �����ϴ� �� lax�� ��ǥ�̹Ƿ� 
	// �̸� ������ ������ ���̺귯���� ���δ�. 
	// flatbuffers�� ����� �������� ���� �����ϰ� 
	// �پ��� �� ������ ���� ���������� lax���� ���� �ʴ�. 
	// 

	SECTION("usage and concepts")
	{

	}

	SECTION("session integration")
	{
		SECTION("serialization")
		{

		}

		SECTION("define message")
		{

		}

		SECTION("buffer reuse")
		{

		}
	}


}