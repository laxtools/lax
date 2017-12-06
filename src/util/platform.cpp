#include "stdafx.h"
#include "platform.h"

namespace lax {
namespace util {

std::string platform::get_current_dir()
{
	CHAR path[MAX_PATH + 1] = "";
	DWORD len = GetCurrentDirectoryA(MAX_PATH, path);

	return path;
}

} // util 
} // lax