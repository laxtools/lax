#pragma once

#include <stdint.h>

/// AES256 algorithm implementation 
/** 
 * From:  https://github.com/kokke/tiny-AES-c
 * 
 * Game needs to protect packet, but most attacks goes to memory 
 * , which means we cannot protect on client side. 
 * 
 * Simple, portable, small per session key is fine. 
 */

namespace aes
{

// AES 128 only

#define AES_BLOCKLEN 16		//Block length in bytes AES is 128b block only
#define AES_KEYLEN 16		// Key length in bytes
#define AES_keyExpSize 176

struct context
{
	uint8_t RoundKey[AES_keyExpSize];
	uint8_t Iv[AES_BLOCKLEN];
};

/// initialize context 
void init(struct context& ctx, const uint8_t* key);

/// initialize context with iv
void init_with_iv(struct context& ctx, const uint8_t* key, const uint8_t* iv);

/// set iv to the context after initialize
void set_iv(struct context& ctx, const uint8_t* iv);

// buffer size MUST be mutile of AES_BLOCKLEN;
// Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
// NOTES: you need to set IV in ctx via AES_init_ctx_iv() or AES_ctx_set_iv()
//        no IV should ever be reused with the same key 

/// encrypt
void encrypt(struct context& ctx, uint8_t* buf, uint32_t length);

/// decrypt
void decrypt(struct context& ctx, uint8_t* buf, uint32_t length);

} // namespace aes

