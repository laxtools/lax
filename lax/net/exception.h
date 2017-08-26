#pragma once 

#include <exception>

namespace lax
{
namespace net
{

struct exception : public std::exception
{
	explicit exception(char const* const m) throw()
		: std::exception(m)
	{
	}
};

} // net
} // lax
