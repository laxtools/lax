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
		// 감동이다! 깔끔한 인터페이스. 숨기기. create의 아규먼트만 교체하면 바뀜

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
			// - blcok cipher 자체론 안전하지 않음 
			// - 긴 데이터에 대해 적용하는 모드
			// - 동일 데이터에 대해 동일 키와 알고리즘으로 변환하면 같은 
			//   데이터가 나오기 때문에 분석에 취약하고 원래 정보를 
			//   노출 시킬 위험이 있어 나중에 추가된 기능 
			// - 단어: nonce (한번 뿐인. 목하. 지금 보는) 
			//   IV에 대해 nonce를 사용하고 있다. 
			// 


			SECTION("IV 처리")
			{
				// 
				// IV (initialization vector / nonce)의 이해 
				// - 송수신 측에서 같아야 한다. 
				// - 암호 모드마다 요구가 다르다. 
				// - CBC는 매번 다르게 줘야 한다. 
				// - 이전 송수신에서 만들어 내는 방식으로 한다. 
				// 

				//
				// KEY 변경과 IV 변경을 동시에 한다. 
				// - sha1 해시 값을 사용한다. 
				// - 데이터 크기는 메세지 일부를 사용한다. 
				// 
			}

			SECTION("padding")
			{
				// Padding의 이해 
				// - PKCS7은 패딩으로 추가되는 길이를 각 패팅에 추가해서 
				// - 안정적으로 패드 처리를 하게 해준다. 
				// - 패드가 4바이트이면 04 04 04 04로 들어간다. 

				// Cipher_Mode가 패딩을 자동으로 처리해 주는가? 
				// - 그렇다. 평문과 암호문의 길이를 보니 패딩이 추가 되었다. 
				// - finish에서 처리한다. 
			}
		}

		SECTION("buffer 관리")
		{
			// 송신 : 
			// - 임시 resize_buffer를 사용한다. (풀에서 할당) 
			// - 길이는 block_size()에 맞춰 align 해 둔다. 
			// - 해당 버퍼로 암호화를 처리 

			// 수신 : 
			// - 고정 resize_buffer인 수신 버퍼를 사용한다. 
			// - 이미 align 되어 있으므로 해당 버퍼를 사용할 수 있다. 

			// 두 가지 모두 가능한데 resize_buffer를 사용할 수 있는가? 
			// - secure_vector로 처리하면 메모리의 내용을 지워준다. 
			// - 이게 더 안전하긴 한데 통신 성능은 떨어질 수 있다. 
			// - 보안이 패킷 보안 비중이 낮으므로 resize_buffer를 재사용 한다. 
			// 
			// secure_vector만 finish()의 인터페이스로 제공한다. 
			// finish 외에 다른 방법으로 암호화를 처리해야 한다. 
			// - process도 in-place 처리를 한다. 단지, granularity가 맞아야 한다. 
			// - finish는 마지막 블럭 처리에 사용한다. 
			// - 패딩 처리가 여기에만 있는 이유이다. 
			// 

			// 
			// 정리하면 다음과 같다: 
			// - 마지막 블럭 전 블럭까지 process로 처리한다. 
			// - 마지막 블럭은 secure_vector에 복사해서 finish를 호출한다. 
			// - AES 블록 크기인 16바이트만 복사가 발생한다.  
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
				// 길이가 딱 맞을 경우 블럭 전체가 패딩이 된다. 
				// 실제 decrypt 코드에서도 패드 길이가 0이면 오류로 처리
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

				// 패딩을 포함한 마지막 블럭 처리
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

				// 이미 align 됨 
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

				// 마지막 블럭 처리를 하면 패딩은 제거된다. 
				dec->finish(final_block);

				// copy to resize_buffer
				// 복사할 때 버퍼가 변경되지 않는다. (버퍼 유지를 위해 의도적)
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
		// configure.py로 botan_all.h와 botan_all.cpp 생성 
		// --amalgamation 옵션
		// --list-modulues로 모듈 확인 
		// --enable-modules=aes,sha1,cbc,crc 로 빌드
		// --minimized-build 
		// --single-amalgamation-file

		// python configure.py --amalgamation --single-amalgamation-file --minimized-build --enable-modules=aes,sha1,cbc,crc32
		
		//
		// android 빌드를 위해서는 별도 구성이 필요
		// - 플래폼 호환을 위한 빌드 구성은 완료 후 처리 
		// - 빌드 환경 셋업을 위해 필요
		//

		//
		// 복사 후 BOTAN_DLL 매크로를 __declspec(dllimport)를 제거해야 함 
		// 아마도 configure.py 옵션에 있을 듯 하니 찾아 볼 것 
		// 
	}
}