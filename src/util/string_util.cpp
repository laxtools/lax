#include "stdafx.h"

#include <lax/util/string_util.hpp>
#include <algorithm>

namespace lax
{
namespace util
{  

const std::string string_util::empty;

void string_util::trim(std::string& str, bool left, bool right)
{
	static const std::string delims = " \t\r";

	if (right)
	{
		str.erase(str.find_last_not_of(delims) + 1); // trim right
	}

	if (left)
	{
		str.erase(0, str.find_first_not_of(delims)); // trim left
	}
}

void string_util::trim(std::wstring& str, bool left, bool right)
{
	static const std::wstring delims = L" \t\r";

	if (right)
	{
		str.erase(str.find_last_not_of(delims) + 1); // trim right
	}

	if (left)
	{
		str.erase(0, str.find_first_not_of(delims)); // trim left
	}
}

bool string_util::convert(const std::string& src, std::wstring& out, uint32_t code_page)
{
	out.clear();

	int slen = std::min<int>(8192, (int)strlen(src.c_str()));
	if (slen == 0)
	{
		return true;
	}

	int wlen = MultiByteToWideChar(code_page, 0, src.c_str(), slen, NULL, NULL);
	LPWSTR np = (LPWSTR)_alloca(sizeof(WCHAR) * (wlen + 1)); // 스택에서 할당
	::memset(np, 0, (wlen + 1) * sizeof(WCHAR));
	MultiByteToWideChar(code_page, 0, src.c_str(), slen, np, wlen);

	out.append(np);

	return true;
}

bool string_util::convert(const std::wstring& s, std::string& out, uint32_t code_page)
{
	out.clear();

	int slen = std::min<int>(8192, (int)wcslen(s.c_str()));

	int wlen = WideCharToMultiByte(code_page, 0, s.c_str(), slen, NULL, 0, NULL, NULL);
	if (wlen == 0)
	{
		return true;
	}

	LPSTR np = (LPSTR)_alloca(sizeof(CHAR) * (wlen + 1));
	::memset(np, 0, (wlen + 1) * sizeof(CHAR));
	WideCharToMultiByte(code_page, 0, s.c_str(), slen, np, wlen, NULL, NULL);

	out.append(np);

	return true;
}

bool string_util::convert(const std::string& src, std::string& out, uint32_t code_page)
{
	std::wstring wstr;
	convert(src, wstr);
	convert(wstr, out, code_page);

	return true;
}

bool string_util::convert(const std::wstring& src, std::wstring& out, uint32_t code_page)
{
	code_page;
	out = src;
	return true;
}

bool string_util::convert(const wchar_t* src, std::wstring& out, uint32_t code_page)
{
	code_page;
	out = src;
	return true;
}

bool string_util::convert(const char* src, std::wstring& out, uint32_t code_page)
{
	out.clear();

	int slen = std::min<int>(8192, (int)strlen(src));
	if (slen == 0)
	{
		return true;
	}

	int wlen = MultiByteToWideChar(code_page, 0, src, slen, NULL, NULL);
	LPWSTR np = (LPWSTR)_alloca(sizeof(WCHAR) * (wlen + 1)); // 스택에서 할당
	::memset(np, 0, (wlen + 1) * sizeof(WCHAR));
	MultiByteToWideChar(code_page, 0, src, slen, np, wlen);

	out.append(np);

	return true;
}

string_util::svec string_util::tokenize(const std::string& str, const std::string& delims, unsigned int max_splits)
{
	svec ret;

	// Pre-allocate some space for performance
	ret.reserve(max_splits ? max_splits + 1 : 10);    // 10 is guessed capacity for most case

	unsigned int num_splits = 0;

	// Use STL methods 
	size_t start = 0; 
	size_t pos = 0;

	do
	{
		pos = str.find_first_of(delims, start);

		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == std::string::npos || (max_splits > 0 && num_splits == max_splits))
		{
			// Copy the rest of the string
			ret.push_back(str.substr(start));
			break;
		}
		else
		{
			// Copy up to delimiter
			ret.push_back(str.substr(start, pos - start));
			start = pos + 1;
		}

		// Parse up to next real data
		start = str.find_first_not_of(delims, start);
		++num_splits;

	} while (pos != std::string::npos);

	return ret;
}

void string_util::tokenize(const std::wstring& msg, wvec& tokens, const std::wstring& delimiter)
{
	std::wstring::size_type last_pos = msg.find_first_not_of(delimiter, 0);
	std::wstring::size_type pos = msg.find_first_of(delimiter, last_pos);

	while (std::wstring::npos != pos || std::wstring::npos != last_pos)
	{
		std::wstring var = msg.substr(last_pos, pos - last_pos);

		tokens.push_back(var);

		last_pos += var.length() + delimiter.length();

		if (last_pos > msg.length())
		{
			last_pos = std::wstring::npos;

			return;
		}

		pos = msg.find_first_of(delimiter, last_pos);
	}
}

std::string string_util::standardize_path(const std::string& init)
{
	std::string path = init;

	std::replace(path.begin(), path.end(), '\\', '/');

	if (path[path.length() - 1] != '/')
	{
		path += '/';
	}

	return path;
}

void string_util::split_path(const std::string& qualifiedName, std::string& outBasename, std::string& outPath)
{
	std::string path = qualifiedName;

	// Replace \ with / first
	std::replace(path.begin(), path.end(), '\\', '/');

	// split based on final /
	size_t i = path.find_last_of('/');

	if (i == std::string::npos)
	{
		outPath.clear();
		outBasename = qualifiedName;
	}
	else
	{
		outBasename = path.substr(i + 1, path.size() - i - 1);
		outPath = path.substr(0, i + 1);
	}
}

void string_util::split_path(const std::wstring& qualifiedName, std::wstring& outBasename, std::wstring& outPath)
{
	std::wstring path = qualifiedName;

	// Replace \ with / first
	std::replace(path.begin(), path.end(), L'\\', L'/');

	// split based on final /
	size_t i = path.find_last_of(L'/');

	if (i == std::wstring::npos)
	{
		outPath.clear();
		outBasename = qualifiedName;
	}
	else
	{
		outBasename = path.substr(i + 1, path.size() - i - 1);
		outPath = path.substr(0, i + 1);
	}
}

void string_util::split_filename(const std::string& fullName, std::string& outBasename, std::string& outExtention)
{
	size_t i = fullName.find_last_of(".");
	if (i == std::string::npos)
	{
		outExtention.clear();
		outBasename = fullName;
	}
	else
	{
		outExtention = fullName.substr(i + 1);
		outBasename = fullName.substr(0, i);
	}
}


} // util
} // lax
