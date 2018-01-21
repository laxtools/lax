#pragma once 

#include <lax/net/packet.hpp>
#include <bitset>

// add this file into stdafx.h for faster parsing
/// to add bitsery specializations
#include "bits_util.hpp"

namespace lax
{
namespace net
{

/// base class for bits messages
/** 
 * Usage: 
 * - subclass
 * - add BITS_BODY macro 
 * - call BITS_ADD( msg ) before using it
 * - define serialize template function in global namespace 
 * - use array, vector only (map is not supported for now)
 */
struct bits_message : public packet
{
	virtual bool pack(packet::ptr m) = 0;
	virtual bool unpack(packet::ptr m) = 0;
};

} // net
} // lax

/// pack / unpack
#define BITS_BODY() 

/// reg to factory
#define BITS_REG(cls, encrypted, checksum)
// bits_protocol::reg(lambda to create cls
// set flag for the class
// 00000011. 2nd bit - encrypted, 1st bit - checksum
//
