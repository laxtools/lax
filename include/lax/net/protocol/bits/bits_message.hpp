#pragma once 

#include <lax/net/packet.hpp>
#include <spdlog/fmt/fmt.h>
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
	std::string desc; 

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

/// macros to define a message 
/**
 * BITS_MSG_TOPIC()
 * - this must come before BITS_MSG_CLASS*
 * - BITS_MSG_CLASS* uses topic_key defined with this
 *
 * BITS_MSG_CLASS() or BITS_MSG_CLASS_DETAIL()
 * - use detail version to setup encryption / checksum option
 * - default setup is false / false
 *
 * BITS_SERIALIZE_TOPIC
 * - use this in serialize function to add topic into serialzed data
 */


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

/// topic_key�� static���� ����. �����ڿ��� ����
#define BITS_MSG_TOPIC(group, type) MSG_TOPIC(group, type) \
static constexpr const char* group_name = #group;  \
static constexpr const char* type_name = #type; 

/// �޼��� �����ڿ� get_desc() ����. ��ȣȭ / üũ���� Ŭ������ ���� 
/** 
 * BITS_MSG_TOPIC�� ���� ȣ��Ǿ�� ��. 
 * 
 * ��ȣȭ / CRC�� ���� ������ ����ġ�� ���� �߻�. 
 * Ŭ���� ������ �� �ְ� ��� ������ ���߸� ��ġ�ϵ��� ��.
 */
#define BITS_MSG_CLASS_DETAIL(cls, enc, crc) \
static constexpr bool enable_cipher = enc; \
static constexpr bool enable_checksum = crc; \
cls() \
: lax::net::bits_message(topic_key) \
{ \
	desc = fmt::format("{}:{}/{}",  \
		#cls, get_topic().get_group(), get_topic().get_type()); \
} \
\
const char* get_desc() const override \
{ \
	return desc.c_str(); \
} \
BITS_MSG_BODY()

/// ��ȣȭ, CRC�� disable�� ���� ����
#define BITS_MSG_CLASS(cls) BITS_MSG_CLASS_DETAIL(cls, false, false)

/// topic_key�� serialize �ϴ� �Լ�.
/**
 * ���� �ʰ� serialize �Լ� �ȿ� �߰��ؾ� ��
 */
#define BITS_SERIALIZE_TOPIC(s, m) \
packet::topic_t::key_t key = m.get_topic().get_key(); \
s.value4b(key); \
m.set_topic(packet::topic_t(key)); 

