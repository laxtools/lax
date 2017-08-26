#include "stdafx.h"

#include <catch.hpp>

#include <array>
#include <iostream>

TEST_CASE("design summary")
{
	SECTION("add transport to session")
	{
		// transport_proto3
		// - ���� : 
		//   - serialize 
		//   - buffer
		// - ���� : 
		//   - arena
		//   - ����
		//     - ��Ŷ ���� 
		//     - ���� ����
		//       - ���� �ʵ� ���� 
		//       - type / �� ���� 
	}

	SECTION("serialization library")
	{
		// flatbuffers 
		// - small and fast 
		// - object Ÿ�� ���� ���� ����� �� ��

		// protobuf 
		// - big
		// - object�� �ʵ� ���� ���� �޸𸮷� ������ (arena ����)

		// msgpack 
		// - ������ ���� ������ ������ ���� schema���� �ڵ� ������ �� ��
	}

	SECTION("����")
	{
		// msgpack + schema ��ģ ������ �̻���
		// - schema --> �ڵ� (������Ʈ + serialize) 
		// - ������ ������Ʈ ��ü�� ���� �ش� ���� ���

		// https://github.com/pascaldekloe/colfer
		// - ���� ���� �õ� �� �ϳ�
		// - ��ü ����
	}
}

TEST_CASE("msgpack generator design")
{
	SECTION("serialization module")
	{
		// 1) c++�� �����ϰ� c# �ڵ带 �����ϴ� ���. 
		//    - visual c++�� ��� �����Ϸ� ���� ������ �� �� 
		//    - clang ���� �� ����ϴ� ����� ������ ��. 

		// 2) �ܺ� ���Ϸ� �����ϰ� c#, c++ �ڵ带 �����ϴ� ���. 
		//    - thrift, protobuf ���� ����ϴ� ���. 
		//    - �ڵ尡 Ÿ�� �� �°� ����ؾ� ��
		//    - protobuf�� java --> wire�� �߰��� (�ڵ� ���� ǰ���� ����)
	}

	SECTION("����")
	{
		// session 
		// - set transport for protocol handling
		// - provide diverse transport 

		// serialization is used by transport
		// - on_recv
		// - on_send
	}
}

