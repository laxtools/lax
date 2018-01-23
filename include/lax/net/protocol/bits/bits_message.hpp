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
	/// type overloading. required for factory
	using ptr = std::shared_ptr<bits_message>; 

	/// header length
	static constexpr std::size_t header_length = 8;

	std::string desc;		/// description. cls:group:type  
	uint32_t	length = 0; /// length �ʵ�. serialization ��ġ Ȯ����  
	uint32_t	topic = 0;	/// for seralization purpose only

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


	/// implementation detail. 

	bool		enable_cipher = false;		
	bool		enable_checksum = false;
	bitsery::ReaderError reader_error = bitsery::ReaderError::NoError;
};

} // net
} // lax


/// macros to define a message 
/**
 * Use BITS_MSG or BITS_MSG_DETAIL
 *
 *
 *
 * BITS_SERIALIZE_TOPIC
 * - use this in serialize function to add topic into serialzed data
 */


 /// pack / unpack
#define BITS_MSG_BODY() \
std::size_t pack(resize_buffer& buf) override \
{ \
	topic = get_topic().get_key(); \
	check(topic > 0); \
	BitsSerializer serializer{ buf }; \
	serializer.value4b(length); \
	serializer.value4b(topic); \
	serializer.object(*this); \
\
	auto& writer = bitsery::AdapterAccess::getWriter(serializer); \
	writer.flush(); \
\
	return writer.writtenBytesCount(); \
} \
\
bool unpack(resize_buffer& buf, resize_buffer::iterator& iter, \
			std::size_t len) override \
{ \
	check(iter != buf.end()); \
	check(len > 0); \
	BitsDeserializer deserializer{ InputAdapter{ iter, len } }; \
	deserializer.value4b(length); \
	deserializer.value4b(topic); \
	deserializer.object(*this); \
	check(topic > 0); \
	\
	auto& reader = bitsery::AdapterAccess::getReader(deserializer); \
	reader_error = reader.error(); \
	\
	set_topic(lax::net::packet::topic_t(topic)); \
	return reader.isCompletedSuccessfully(); \
}

/// topic_key�� static���� ����. �����ڿ��� ����
/*
 * BITS_MSG_TOPIC()
 * - this must come before BITS_MSG_CLASS*
 * - BITS_MSG_CLASS* uses topic_key defined with this
 */
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
cls() \
: lax::net::bits_message(topic_key) \
{ \
	desc = fmt::format("{}:{}:{}",  \
		#cls, get_topic().get_group(), get_topic().get_type()); \
	enable_cipher = enc; \
	enable_checksum = crc; \
} \
\
const char* get_desc() const override \
{ \
	return desc.c_str(); \
} \
BITS_MSG_BODY()

 /// ��ȣȭ, CRC�� disable�� ���� ����
#define BITS_MSG_CLASS(cls) BITS_MSG_CLASS_DETAIL(cls, false, false)


/// ������ ����. 
#define BITS_MSG(group, type, cls) \
	BITS_MSG_TOPIC(group, type); \
	BITS_MSG_CLASS(cls)

/// Ǯ ����. ��ȣȭ, crc üũ �ɼ� �߰�
#define BITS_MSG_DETAIL(group, type, cls, enc, crc) \
	BITS_MSG_TOPIC(group, type); \
	BITS_MSG_CLASS_DETAIL(cls, enc, crc)


#define BITS_SERIALIZE(...) \
template <typename S> \
void serialize(S& s) \
{ \
	s.archive( \
		__VA_ARGS__ \
	); \
}
