#include "stdafx.h"
#include <catch.hpp>
#include <lax/util/botan/botan_all.h>
#include <lax/net/detail/buffer/resize_buffer.hpp>
#include <iostream>

using namespace lax::net;

TEST_CASE("botan research")
{
	SECTION("hash: crc32 / sha1")
	{
		// �����̴�! ����� �������̽�. �����. create�� �ƱԸ�Ʈ�� ��ü�ϸ� �ٲ�

		std::unique_ptr<Botan::HashFunction> hash1(Botan::HashFunction::create("CRC32"));

		std::vector<uint8_t> buf{ 'a', 1, 23, 4, 5, 6, 7 };

		//update hash computations with read data
		hash1->update(buf.data(), buf.size());

		std::cout << "CRC32: " << Botan::hex_encode(hash1->final()) << std::endl;
	}

	SECTION("aes")
	{
		// cbc mode 
		// padding

		// interface and integration w/ cipher_modifier

		SECTION("usage")
		{
			const std::string plaintext(
				"Your great-grandfather gave this watch to your granddad "
				"for good luck. Unfortunately, Dane's luck wasn't as good as"
				"his old man's."
			);
			const std::vector<uint8_t> key = Botan::hex_decode(
				"2B7E151628AED2A6ABF7158809CF4F3C"
			);
			const std::vector<uint8_t> nonce = Botan::hex_decode(
				"2B7E151628AED2A6ABF7158809CF4F3C"
			);

		}

		SECTION("aes block cipher")
		{

		}

		SECTION("cipher mode")
		{
			const std::string plaintext(
				"Your great-grandfather gave this watch to your granddad for "
				"good luck. Unfortunately, Dane's luck wasn't as good as his "
				"old man's."
			);
			const std::vector<uint8_t> key = Botan::hex_decode(
				"2B7E151628AED2A6ABF7158809CF4F3C"
			);
			const std::vector<uint8_t> nonce = Botan::hex_decode(
				"2B7E151628AED2A6ABF7158809CF4F3C"
			);

			std::unique_ptr<Botan::Cipher_Mode> enc(
				Botan::get_cipher_mode("AES-128/CBC/PKCS7", Botan::ENCRYPTION)
			);

			enc->set_key(key);

			Botan::secure_vector<uint8_t> pt(
				plaintext.data(), plaintext.data() + plaintext.length()
			);

			std::cout << "original size: " << pt.size() << std::endl;;

			//generate fresh nonce (IV)
			enc->start(nonce);
			enc->finish(pt);

			std::cout << "encrypted size: " << pt.size() << std::endl;;

			std::cout << enc->name() << " with iv " << Botan::hex_encode(nonce) 
				<< " " << Botan::hex_encode(pt) << "\n";
		}

		SECTION("concepts")
		{
			// Cipher_Mode 
			// - blcok cipher ��ü�� �������� ���� 
			// - �� �����Ϳ� ���� �����ϴ� ���
			// - ���� �����Ϳ� ���� ���� Ű�� �˰������� ��ȯ�ϸ� ���� 
			//   �����Ͱ� ������ ������ �м��� ����ϰ� ���� ������ 
			//   ���� ��ų ������ �־� ���߿� �߰��� ��� 
			// - �ܾ�: nonce (�ѹ� ����. ����. ���� ����) 
			//   IV�� ���� nonce�� ����ϰ� �ִ�. 
			// 


			SECTION("IV ó��")
			{
				// 
				// IV (initialization vector / nonce)�� ���� 
				// - �ۼ��� ������ ���ƾ� �Ѵ�. 
				// - ��ȣ ��帶�� �䱸�� �ٸ���. 
				// - CBC�� �Ź� �ٸ��� ��� �Ѵ�. 
				// - ���� �ۼ��ſ��� ����� ���� ������� �Ѵ�. 
				// 

				//
				// KEY ����� IV ������ ���ÿ� �Ѵ�. 
				// - sha1 �ؽ� ���� ����Ѵ�. 
				// - ������ ũ��� �޼��� �Ϻθ� ����Ѵ�. 
				// 
			}

			SECTION("padding")
			{
				// Padding�� ���� 
				// - PKCS7�� �е����� �߰��Ǵ� ���̸� �� ���ÿ� �߰��ؼ� 
				// - ���������� �е� ó���� �ϰ� ���ش�. 
				// - �е尡 4����Ʈ�̸� 04 04 04 04�� ����. 

				// Cipher_Mode�� �е��� �ڵ����� ó���� �ִ°�? 
				// - �׷���. �򹮰� ��ȣ���� ���̸� ���� �е��� �߰� �Ǿ���. 
				// - finish���� ó���Ѵ�. 
			}
		}

		SECTION("buffer ����")
		{
			// �۽� : 
			// - �ӽ� resize_buffer�� ����Ѵ�. (Ǯ���� �Ҵ�) 
			// - ���̴� block_size()�� ���� align �� �д�. 
			// - �ش� ���۷� ��ȣȭ�� ó�� 

			// ���� : 
			// - ���� resize_buffer�� ���� ���۸� ����Ѵ�. 
			// - �̹� align �Ǿ� �����Ƿ� �ش� ���۸� ����� �� �ִ�. 

			// �� ���� ��� �����ѵ� resize_buffer�� ����� �� �ִ°�? 
			// - secure_vector�� ó���ϸ� �޸��� ������ �����ش�. 
			// - �̰� �� �����ϱ� �ѵ� ��� ������ ������ �� �ִ�. 
			// - ������ ��Ŷ ���� ������ �����Ƿ� resize_buffer�� ���� �Ѵ�. 
			// 
			// secure_vector�� finish()�� �������̽��� �����Ѵ�. 
			// finish �ܿ� �ٸ� ������� ��ȣȭ�� ó���ؾ� �Ѵ�. 
			// - process�� in-place ó���� �Ѵ�. ����, granularity�� �¾ƾ� �Ѵ�. 
			// - finish�� ������ �� ó���� ����Ѵ�. 
			// - �е� ó���� ���⿡�� �ִ� �����̴�. 
			// 

			// 
			// �����ϸ� ������ ����: 
			// - ������ �� �� ������ process�� ó���Ѵ�. 
			// - ������ ���� secure_vector�� �����ؼ� finish�� ȣ���Ѵ�. 
			// - AES ��� ũ���� 16����Ʈ�� ���簡 �߻��Ѵ�.  
			//

			const std::string plaintext(
				"Your great-grandfather gave this watch to your granddad for "
				"good luck. Unfortunately, Dane's luck wasn't as good as his "
				"old man's.12345678901234"
			);
			const std::vector<uint8_t> key = Botan::hex_decode(
				"2B7E151628AED2A6ABF7158809CF4F3C"
			);
			const std::vector<uint8_t> nonce = Botan::hex_decode(
				"2B7E151628AED2A6ABF7158809CF4F3C"
			);

			resize_buffer plain_buf;

			const std::size_t BS = Botan::AES_128::BLOCK_SIZE;

			// encryption
			{
				std::unique_ptr<Botan::Cipher_Mode> enc(
					Botan::get_cipher_mode("AES-128/CBC/PKCS7", Botan::ENCRYPTION)
				);

				enc->set_key(key);

				plain_buf.append(plaintext.c_str(), plaintext.size());

				// add padding size to ensure buffer size for padding
				// ���̰� �� ���� ��� �� ��ü�� �е��� �ȴ�. 
				// ���� decrypt �ڵ忡���� �е� ���̰� 0�̸� ������ ó��
				auto pad_size = BS - plain_buf.size() % BS;
				const char* pad = "2B7E151628AED2A6ABF7158809CF4F3C";
				plain_buf.append(pad, pad_size);

				REQUIRE(plain_buf.size() % BS == 0);

				std::cout << "original size: " << plain_buf.size() << std::endl;;

				Botan::secure_vector<uint8_t> final_block;

				//generate fresh nonce (IV)
				enc->start(nonce);

				std::size_t fin_pos_begin = plain_buf.size() - BS;
				std::size_t pro_pos_end = fin_pos_begin;

				if (pro_pos_end < plain_buf.size())
				{
					enc->process(plain_buf.data(), pro_pos_end);
				}

				final_block.reserve(BS);

				// copy to final_block
				// only copy bytes except pads
				std::copy(
					plain_buf.begin() + (ptrdiff_t)fin_pos_begin,
					plain_buf.end() - (ptrdiff_t)pad_size, 
					std::back_inserter(final_block)
				);

				// �е��� ������ ������ �� ó��
				enc->finish(final_block);

				REQUIRE(final_block.size() == BS);

				// copy to resize_buffer
				std::memcpy(
					plain_buf.data() + fin_pos_begin,
					final_block.data(),
					BS
				);
			}

			// decryption
			{
				std::unique_ptr<Botan::Cipher_Mode> dec(
					Botan::get_cipher_mode("AES-128/CBC/PKCS7", Botan::DECRYPTION)
				);

				dec->set_key(key);

				// �̹� align �� 
				REQUIRE(plain_buf.size() % BS == 0);

				std::cout << "encrypted size: " << plain_buf.size() << std::endl;;

				Botan::secure_vector<uint8_t> final_block;

				//generate fresh nonce (IV)
				dec->start(nonce);

				std::size_t fin_pos_begin = plain_buf.size() - BS;
				std::size_t pro_pos_end = fin_pos_begin;

				if (pro_pos_end < plain_buf.size())
				{
					dec->process(plain_buf.data(), pro_pos_end);
				}

				final_block.reserve(BS);

				// copy to final_block
				std::copy(
					plain_buf.begin() + (ptrdiff_t)fin_pos_begin,
					plain_buf.end(),
					std::back_inserter(final_block)
				);

				// ������ �� ó���� �ϸ� �е��� ���ŵȴ�. 
				dec->finish(final_block);

				// copy to resize_buffer
				// ������ �� ���۰� ������� �ʴ´�. (���� ������ ���� �ǵ���)
				std::memcpy(
					plain_buf.data() + fin_pos_begin,
					final_block.data(),
					final_block.size()
				);
			}
		}
	}

	SECTION("obfusfication")
	{
		// https://github.com/fritzone/obfy
		// - use it to hash data generation
		// - make it hard to understand in assembly
	}

	SECTION("building")
	{
		// configure.py�� botan_all.h�� botan_all.cpp ���� 
		// --amalgamation �ɼ�
		// --list-modulues�� ��� Ȯ�� 
		// --enable-modules=aes,sha1,cbc,crc �� ����
		// --minimized-build 
		// --single-amalgamation-file

		// python configure.py --amalgamation --single-amalgamation-file --minimized-build --enable-modules=aes,sha1,cbc,crc32
		
		//
		// android ���带 ���ؼ��� ���� ������ �ʿ�
		// - �÷��� ȣȯ�� ���� ���� ������ �Ϸ� �� ó�� 
		// - ���� ȯ�� �¾��� ���� �ʿ�
		//

		//
		// ���� �� BOTAN_DLL ��ũ�θ� __declspec(dllimport)�� �����ؾ� �� 
		// �Ƹ��� configure.py �ɼǿ� ���� �� �ϴ� ã�� �� �� 
		// 
	}
}