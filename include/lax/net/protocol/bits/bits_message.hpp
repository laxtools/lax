#pragma once 

#include <lax/net/message.hpp>

// add this file into stdafx.h for faster parsing

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
struct bits_message : public message
{
	virtual bool pack(message::ptr m) = 0;
	virtual bool unpack(message::ptr m) = 0;
};

} // net
} // lax

/// pack / unpack
#define BITS_BODY() 

/// reg to factory
#define BITS_REG(m)

/// to add bitsery specializations
#include "bits_util.hpp"