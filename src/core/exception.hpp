#pragma once 

#include <exception>
#include <spdlog/fmt/fmt.h>

namespace lax
{
namespace util
{

class exception : public std::exception
{
public: 
	explicit exception(char const* const m, const char* file, int line) throw()
		: std::exception(m)
	{
		desc_ = fmt::format("exception: {} on {}:{}", std::exception::what(), file, line);
	}

	virtual char const* what() const override
	{
		return desc_.c_str();
	}

private:
	std::string desc_;
};

} // util
} // lax

#define THROW(m) 	throw lax::util::exception(m, __FILE__, __LINE__)
