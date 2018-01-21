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
	bits_message(const topic_t::key_t key)
		: bits_message(topic_t(key))
	{
	}

	bits_message(const topic_t& topic)
		: packet(topic)
	{
	}

	virtual std::size_t pack(
		resize_buffer& buf
	) = 0;

	virtual bool unpack(
		resize_buffer& buf,
		resize_buffer::iterator& iter,
		std::size_t len
	) = 0;
};

} // net
} // lax

template <typename S>
void serialize(S& s, lax::net::bits_message& o)
{
	lax::net::packet::topic_t::key_t key = o.topic_;
	s.value4b(key);
	o.topic_ = key;
}


/// pack / unpack
#define BITS_MSG_BODY() \
std::size_t pack(resize_buffer& buf) override \
{ \
	BitsSerializer serializer{ buf }; \
	serializer.object(*this); \
\
	auto& writer = bitsery::AdapterAccess::getWriter(serializer); \
	writer.flush(); \
\
	return writer.writtenBytesCount(); \
} \
\
bool unpack(resize_buffer& buf, resize_buffer::iterator& iter, std::size_t len) override \
{ \
	check(iter != buf.end()); \
	check(len > 0); \
	BitsDeserializer deserializer{ InputAdapter{ iter, len } }; \
	deserializer.object(*this); \
\
	auto& reader = bitsery::AdapterAccess::getReader(deserializer); \
	auto error = reader.error(); \
	return reader.isCompletedSuccessfully(); \
}

#define BITS_MSG_TOPIC(group, type) MSG_TOPIC(group, type)
#define BITS_MSG_CLASS(cls) \
cls() \
: lax::net::bits_message(topic_key) \
{ \
} \
\
const char* get_desc() const override \
{ \
	return #cls; \
} \
BITS_MSG_BODY()

#define BITS_SERIALIZE_TOPIC(s, m) \
packet::topic_t::key_t key = m.get_topic().get_key(); \
s.value4b(key); \
m.set_topic(packet::topic_t(key)); 

/// reg to factory
#define BITS_MSG_REG(cls)
// bits_protocol::reg(lambda to create cls
// set flag for the class
// 00000011. 2nd bit - encrypted, 1st bit - checksum
//

#define BITS_MSG_REG_DETAIL(cls, enc, crc)
