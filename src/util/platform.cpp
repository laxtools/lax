#include "stdafx.h"
#include <lax/util/platform.hpp>
#include <lax/util/macros.hpp>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

namespace lax {
namespace util {

std::string platform::get_current_dir()
{
	CHAR path[MAX_PATH + 1] = "";
	DWORD len = GetCurrentDirectoryA(MAX_PATH, path);

	UNUSED(len);

	return path;
}

} // util 
} // lax