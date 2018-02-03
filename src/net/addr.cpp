#include "stdafx.h"
#include <lax/net/addr.hpp>

namespace lax
{
namespace net
{

addr::addr(const std::string& s)
	: raw_(s)
{
	std::size_t pos = s.find_first_of(':');

	RETURN_IF(pos == std::string::npos);
	RETURN_IF(pos + 1 >= s.length());

	auto sip = s.substr(0, pos);
	auto sport = s.substr(pos+1);
	auto port = ::atoi(sport.c_str());

	RETURN_IF(port == 0);

	asio::error_code ec;

	ep_ = tcp::endpoint(asio::ip::address::from_string(sip, ec), port);

	if (!ec)
	{
		valid_ = true;
	}
}

} // net
} // lax
