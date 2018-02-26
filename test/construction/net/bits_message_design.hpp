#pragma once

#include <lax/net/protocol/bits/bits_message.hpp>

using namespace lax::net;

namespace
{

// design bits_message
struct bm : public packet
{
	bm()
		: packet(3)
	{
	}

	std::size_t pack(resize_buffer& buf)
	{
		BitsSerializer serializer{ buf };
		serializer.object(*this);

		auto& writer = bitsery::AdapterAccess::getWriter(serializer);
		writer.flush();

		return writer.writtenBytesCount();
	}

	bool unpack(resize_buffer::iterator& iter, std::size_t len)
	{
		VERIFY(len > 0);

		BitsDeserializer deserializer{ InputAdapter{ iter, len } };
		deserializer.object(*this);

		auto& reader = bitsery::AdapterAccess::getReader(deserializer);

		auto error = reader.error();
		return error == bitsery::ReaderError::NoError;
	}

	int v = 0;
};

template <typename S>
void serialize(S& s, bm& o)
{
	s.value4b(o.v);
}

struct bm2 : public packet
{
	bm2()
		: packet(3)
	{
	}

	std::size_t pack(resize_buffer& buf)
	{
		BitsSerializer serializer{ buf };
		serializer.object(*this);

		auto& writer = bitsery::AdapterAccess::getWriter(serializer);
		writer.flush();

		return writer.writtenBytesCount();
	}

	bool unpack(resize_buffer& buf, resize_buffer::iterator iter, std::size_t len)
	{
		VERIFY(iter != buf.end());
		VERIFY(len > 0);
		// iter에 대한 추가 검증

		BitsDeserializer deserializer{ InputAdapter{ iter, len } };
		deserializer.object(*this);

		auto& reader = bitsery::AdapterAccess::getReader(deserializer);
		auto error = reader.error();
		UNUSED(error);
		return reader.isCompletedSuccessfully();
	}

	std::string name;
};

template <typename S>
void serialize(S& s, bm2& o)
{
	s.text1b(o.name, 64);
}

} // noname