#include "stdafx.h"
#include <catch.hpp>
#include <lax/net/service.hpp>
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
		check(len > 0);

		BitsDeserializer deserializer{ InputAdapter{ iter, len} };
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

} // noname

TEST_CASE("bits protocol")
{
	SECTION("defining messages")
	{
		// design test
		// - implementation of pack/unpack
		// - recv message packetization
		// 

		SECTION("pack / unpack")
		{
			auto mp = std::make_shared<bm>();
			mp->v = 3;

			resize_buffer buf;

			auto size = mp->pack(buf);
			REQUIRE(size > 0);

			auto res = std::make_shared<bm>();
			auto rc = res->unpack(buf.begin(), size);

			REQUIRE(res->v == 3);
		}
	}

	SECTION("communication w/ lambda callbacks")
	{
		auto& svc = service::inst();

		svc.init();



		svc.fini();
	}

	SECTION("performance")
	{
		// single connection send / recv 
		// aims for 1G bps, 1M pps
	}
}