#include "stdafx.h"
#include <lax/net/protocol/util/cipher.hpp>
#include <lax/net/protocol/bits/bits_message.hpp>
#include <lax/util/botan/botan_all.h>

namespace lax
{
namespace net
{

cipher::config cipher::cfg;

constexpr std::size_t BLOCK_SIZE = Botan::AES_128::BLOCK_SIZE;

struct cipher_impl
{
	std::vector<uint8_t> key;
	std::vector<uint8_t> nonce;
	std::vector<uint8_t> data;
	std::size_t			 count = 0;

	Botan::Cipher_Dir	direction;

	std::unique_ptr<Botan::HashFunction> hasher; 

	cipher_impl(Botan::Cipher_Dir dir)
		: hasher(Botan::HashFunction::create("SHA1"))
	{
		UNUSED(dir);
	}

	void update_hash(resize_buffer& buf, std::size_t msg_pos, std::size_t msg_len);

	void update_key();

	void complete();
};

cipher::cipher()
{
}

cipher::~cipher()
{
}

modifier::result cipher::on_bind()
{
	VERIFY(!bound_);

	receiver_ = std::make_unique<cipher_impl>(Botan::Cipher_Dir::DECRYPTION);
	sender_ = std::make_unique<cipher_impl>(Botan::Cipher_Dir::ENCRYPTION);

	receiver_->key	= Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C");
	sender_->key	= Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C");

	receiver_->nonce= Botan::hex_decode("ACE03D519F3CBA2BF67CF1B7E1C4F02D");
	sender_->nonce	= Botan::hex_decode("ACE03D519F3CBA2BF67CF1B7E1C4F02D");

	return result(true, reason::success);
}

modifier::result cipher::on_recv(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len,
	std::size_t& new_len
)
{
	new_len = msg_len;

	// header를 제외하고 암호화 
	VERIFY(msg_len >= bits_message::header_length);

	auto payload_size = msg_len - bits_message::header_length;
	RETURN_IF(payload_size == 0, result(true, reason::success)); // no data 

	// 암호화 할 때 블럭에 맞춤
	VERIFY(payload_size % BLOCK_SIZE == 0);

	auto cipher_len = msg_len - bits_message::header_length;
	auto cipher_pos = msg_pos + bits_message::header_length;
	auto final_pos = cipher_pos + cipher_len - BLOCK_SIZE;
	uint8_t* cipher_ptr = buf.data() + cipher_pos;

	std::size_t pad_size = 0;

	static thread_local std::unique_ptr<Botan::Cipher_Mode> algo(
		Botan::get_cipher_mode("AES-128/CBC/PKCS7", Botan::DECRYPTION)
	);

	// recv는 락 불필요
	{

		algo->clear();

		algo->start(receiver_->nonce);

		algo->set_key(receiver_->key);
	}

	// decryption
	{
		std::size_t process_len = cipher_len - BLOCK_SIZE;

		if (process_len > 0)
		{
			algo->process(cipher_ptr, process_len);
		}

		Botan::secure_vector<uint8_t> final_block;
		final_block.reserve(BLOCK_SIZE);

		std::copy(
			buf.begin() + (ptrdiff_t)final_pos,
			buf.begin() + (ptrdiff_t)(final_pos + BLOCK_SIZE),
			std::back_inserter(final_block)
		);

		algo->finish(final_block);

		std::memcpy(
			buf.data() + final_pos,
			final_block.data(),
			final_block.size()
		);

		pad_size = BLOCK_SIZE - final_block.size();
	}

	// 락 불필요 (한번에 하나씩 처리됨)
	{
		receiver_->update_hash(
			buf,
			msg_pos + bits_message::header_length,
			msg_len - bits_message::header_length - pad_size
		);

		receiver_->complete();
	}

	new_len = msg_len - pad_size;

	return result(true, reason::success);
}

modifier::result cipher::on_send(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	// header를 제외하고 암호화 
	VERIFY(msg_len >= bits_message::header_length);

	auto payload_size = msg_len - bits_message::header_length;
	RETURN_IF(payload_size == 0, result(true, reason::success)); // no data 

	auto pad_size = BLOCK_SIZE - payload_size % BLOCK_SIZE;

	const auto pad = Botan::hex_decode("12345678901234567890123456789012");
	buf.append(pad.data(), pad_size);

	auto cipher_len		= payload_size + pad_size;					// 암호화 길이
	auto cipher_pos		= msg_pos + bits_message::header_length;	// 암호화 시작 위치
	auto final_pos		= cipher_pos + cipher_len - BLOCK_SIZE;		// 최종 블럭 위치	
	uint8_t* cipher_ptr = buf.data() + cipher_pos;					// 암호화 시작 포인터

	static thread_local std::unique_ptr<Botan::Cipher_Mode> algo(
		Botan::get_cipher_mode("AES-128/CBC/PKCS7", Botan::ENCRYPTION)
	);

	// lock 
	{
		std::lock_guard<std::recursive_mutex> lock(send_mutex_);

		algo->clear(); 

		algo->set_key(sender_->key);

		algo->start(sender_->nonce);

		sender_->update_hash(
			buf,
			msg_pos + bits_message::header_length,
			msg_len - bits_message::header_length
		);
	}

	// encryption 
	{
		std::size_t process_len = cipher_len - BLOCK_SIZE;

		if (process_len > 0)
		{
			algo->process(cipher_ptr, process_len);
		}

		Botan::secure_vector<uint8_t> final_block;
		final_block.reserve(BLOCK_SIZE);

		std::copy(
			buf.begin() + (ptrdiff_t)final_pos,
			buf.begin() + (ptrdiff_t)(final_pos + BLOCK_SIZE - pad_size),
			std::back_inserter(final_block)
		);

		algo->finish(final_block);

		VERIFY(final_block.size() == BLOCK_SIZE);

		std::memcpy(
			buf.data() + final_pos,
			final_block.data(),
			final_block.size()
		);
	}

	// lock 
	{
		std::lock_guard<std::recursive_mutex> lock(send_mutex_);

		sender_->complete();
	}

	update_length_field(buf, msg_pos, msg_len + pad_size);

	return result(true, reason::success);
}

void cipher_impl::update_hash(
	resize_buffer& buf, std::size_t msg_pos, std::size_t msg_len)
{
	const uint8_t* pd = buf.data() + msg_pos; // msg_pos가 header 길이를 반영 바뀌어 옴

	auto len = std::min<std::size_t>(msg_len, 64);

	hasher->update(pd, len);
	hasher->update(data);
	hasher->final(data);

	// 성능 보다 보안이 더 중요하여 CRC32대신 SHA1을 사용한다. 
}

void cipher_impl::complete()
{
	nonce.clear();
	std::copy(data.begin(), data.begin() + 16, std::back_inserter(nonce));

	// count가 0일 때도 변경
	if (count % cipher::cfg.key_change_interval == 0)
	{
		update_key();
	}

	count++;
}

void cipher_impl::update_key()
{
	key.clear();
	std::copy(data.begin(), data.begin() + 16, std::back_inserter(key));
}

} // net
} // lax