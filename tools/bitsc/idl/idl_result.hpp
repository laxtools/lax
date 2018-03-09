#pragma once

#include <string>

namespace bitsc
{

struct result
{
	enum error
	{
		success,
		fail_object_type_not_found,
		fail_object_type_invalid,
		fail_object_name_not_found,
		fail_object_type_not_supported
	};

	std::string token;
	std::string desc;
	error rc;

	operator bool() const
	{
		return rc == error::success;
	}
};

} // bitsc