#pragma once 

#include <exception>

namespace lax
{
namespace task
{

struct exception_task : public std::exception
{
	exception_task(const char* what)
		: std::exception(what)
	{
	}
};

struct exception_task_not_ready : public exception_task
{
	exception_task_not_ready(const char* what)
		: exception_task(what)
	{
	}
};

struct exception_task_invalid_affinity : public exception_task
{
	exception_task_invalid_affinity(const char* what)
		: exception_task(what)
	{
	}
};

} // task
} // lax