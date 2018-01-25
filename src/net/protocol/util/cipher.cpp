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

struct cipher_helper
{
	std::vector<uint8_t> key;
	std::vector<uint8_t> nonce;
	std::vector<uint8_t> data;
	std::size_t			 count = 0;

	Botan::Cipher_Dir	direction;
	std::unique_ptr<Botan::Cipher_Mode> algo;
	std::unique_ptr<Botan::HashFunction> hasher; 

	cipher_helper(Botan::Cipher_Dir dir)
		: algo(Botan::get_cipher_mode("AES-128/CBC/PKCS7", dir))
		, direction(dir)
		, hasher(Botan::HashFunction::create("SHA1"))
	{
	}

	void update_hash(resize_buffer& buf, std::size_t msg_pos, std::size_t msg_len);

	void update_key();

	void complete();
};

// therea_local here has an issue since protocol::send and recv called
// from several threads. the object are different for this 

cipher::cipher()
{
}

cipher::~cipher()
{
}

modifier::result cipher::on_bind()
{
	receiver_ = std::make_unique<cipher_helper>(Botan::Cipher_Dir::DECRYPTION);
	sender_ = std::make_unique<cipher_helper>(Botan::Cipher_Dir::ENCRYPTION);

	receiver_->key	= Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C");
	sender_->key	= Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C");

	receiver_->nonce= Botan::hex_decode("ACE03D519F3CBA2BF67CF1B7E1C4F02D");
	sender_->nonce	= Botan::hex_decode("ACE03D519F3CBA2BF67CF1B7E1C4F02D");

	receiver_->algo->set_key(receiver_->key);
	sender_->algo->set_key(sender_->key);

	// TODO: send a message through protocol to change key / nonce
	// - random data 

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
	check(msg_len >= bits_message::header_length);

	auto payload_size = msg_len - bits_message::header_length;
	return_if(payload_size == 0, result(true, reason::success)); // no data 

	// 암호화 할 때 블럭에 맞춤
	check(payload_size % BLOCK_SIZE == 0);

	auto cipher_len = msg_len - bits_message::header_length;
	auto cipher_pos = msg_pos + bits_message::header_length;
	auto final_pos = cipher_pos + cipher_len - BLOCK_SIZE;
	uint8_t* cipher_ptr = buf.data() + cipher_pos;
	
	std::size_t pad_size = 0;

	// lock 
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		receiver_->algo->start(receiver_->nonce);

		std::size_t process_len = cipher_len - BLOCK_SIZE;

		if (process_len > 0)
		{
			receiver_->algo->process(cipher_ptr, process_len);
		}

		Botan::secure_vector<uint8_t> final_block;
		final_block.reserve(BLOCK_SIZE);

		std::copy(
			buf.begin() + (ptrdiff_t)final_pos,
			buf.begin() + (ptrdiff_t)(final_pos + BLOCK_SIZE),
			std::back_inserter(final_block)
		);

		receiver_->algo->finish(final_block);

		std::memcpy(
			buf.data() + final_pos,
			final_block.data(),
			final_block.size()
		);

		pad_size = BLOCK_SIZE - final_block.size();

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
	check(msg_len >= bits_message::header_length);

	auto payload_size = msg_len - bits_message::header_length;
	return_if(payload_size == 0, result(true, reason::success)); // no data 

	auto pad_size = BLOCK_SIZE - payload_size % BLOCK_SIZE;

	const auto pad = Botan::hex_decode("12345678901234567890123456789012");
	buf.append(pad.data(), pad_size);

	auto cipher_len		= payload_size + pad_size;					// 암호화 길이
	auto cipher_pos		= msg_pos + bits_message::header_length;	// 암호화 시작 위치
	auto final_pos		= cipher_pos + cipher_len - BLOCK_SIZE;		// 최종 블럭 위치	
	uint8_t* cipher_ptr = buf.data() + cipher_pos;					// 암호화 시작 포인터

	// lock 
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		sender_->update_hash(
			buf,
			msg_pos + bits_message::header_length,
			msg_len - bits_message::header_length
		);

		sender_->algo->start(sender_->nonce);

		std::size_t process_len = cipher_len - BLOCK_SIZE;

		if (process_len > 0)
		{
			sender_->algo->process(cipher_ptr, process_len);
		}

		Botan::secure_vector<uint8_t> final_block;
		final_block.reserve(BLOCK_SIZE);

		std::copy(
			buf.begin() + (ptrdiff_t)final_pos,
			buf.begin() + (ptrdiff_t)(final_pos + BLOCK_SIZE - pad_size),
			std::back_inserter(final_block)
		);

		sender_->algo->finish(final_block);

		check(final_block.size() == BLOCK_SIZE);

		std::memcpy(
			buf.data() + final_pos,
			final_block.data(),
			final_block.size()
		);

		sender_->complete();
	}

	update_length(buf, msg_pos, msg_len + pad_size);

	return result(true, reason::success);
}

void cipher_helper::update_hash(
	resize_buffer& buf, std::size_t msg_pos, std::size_t msg_len)
{
	const uint8_t* pd = buf.data() + msg_pos; // msg_pos가 header 길이를 반영 바뀌어 옴

	auto len = std::min<std::size_t>(msg_len, 64);

	hasher->update(pd, len);
	hasher->update(data);
	hasher->final(data);
}

void cipher_helper::complete()
{
	nonce.clear();
	std::copy(data.begin(), data.begin() + 16, std::back_inserter(nonce));

	// count가 0일 때도 변경
	if (count % cipher::cfg.key_change_interval == 0)
	{
		algo->clear();

		update_key();

		algo->set_key(key);
	}

	count++;
}

void cipher_helper::update_key()
{
	key.clear();
	std::copy(data.begin(), data.begin() + 16, std::back_inserter(key));
}

} // net
} // lax

  // TODO: optimization 
  //
  // 상태를 분리하면 process()와 finish()를 
  // 락에서 제외할 수 있을 것으로 보인다. 
  // 암호화 객체 자체는 TLS로 만들고 
  // 상태 업데이트만 락을 걸면 좀 더 효율적이 된다. 
  //
  // 성능 측정 후에 추가 작업을 한다. 