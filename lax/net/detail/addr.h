#pragma once 

#include <lax/util/macros.h>

#include <asio.hpp>
#include <string>

using namespace asio::ip;

namespace lax
{
namespace net
{

class addr
{
public:
	addr(const std::string& s);

	addr(const addr& rhs) = default;

	const std::string& get_raw() const
	{
		return raw_;
	}

	tcp::endpoint get_endpoint() const
	{
		return ep_;
	}

	unsigned short port() const
	{
		return ep_.port();
	}

	std::string ip() const
	{
		return ep_.address().to_string();
	}

	/// 초간단 유효성 체크 결과
	bool is_valid() const
	{
		return valid_;
	}

private:
	std::string	raw_;
	tcp::endpoint ep_;
	bool valid_ = false;
};

} // net
} // lax
