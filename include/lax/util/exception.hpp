#pragma once 

#include <exception>

namespace lax
{
namespace util
{

class exception : public std::exception
{
public: 
	explicit exception(char const* const m, const char* file, int line) throw()
		: std::exception(m)
		, file_(file)
		, line_(line)
	{
	}

	const char* get_file() const 
	{
		return file_;
    }

	int get_line() const 
	{
	 	return line_;
	}

private:
	const char* file_ = nullptr;
	int line_ = 0;
};

} // util
} // lax

#define THROW(m) 	throw lax::util::exception(m, __FILE__, __LINE__)
