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
		str.erase(str.find_last_not_of(delims) + 1); // trim right

	if (left)
		str.erase(0, str.find_first_not_of(delims)); // trim left
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

string_util::string_vector string_util::split(const std::string& str, const std::string& delims, unsigned int max_splits)
{
	string_vector ret;

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

void string_util::tokenize(const std::wstring& msg, std::vector<std::wstring>& tokens, const std::wstring& delimiter)
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
		}

		pos = msg.find_first_of(delimiter, last_pos);
	}
}

string_util::string_vector string_util::tokenize(const std::string& str, const std::string& singleDelims, const std::string& doubleDelims, unsigned int maxSplits)
{
	string_vector ret;
	// Pre-allocate some space for performance
	ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

	unsigned int num_splits = 0;
	std::string delims = singleDelims + doubleDelims;

	// Use STL methods 
	size_t start, pos;
	char curDoubleDelim = 0;
	start = 0;
	do
	{
		if (curDoubleDelim != 0)
		{
			pos = str.find(curDoubleDelim, start);
		}
		else
		{
			pos = str.find_first_of(delims, start);
		}

		if (pos == start)
		{
			char curDelim = str.at(pos);
			if (doubleDelims.find_first_of(curDelim) != std::string::npos)
			{
				curDoubleDelim = curDelim;
			}
			// Do nothing
			start = pos + 1;
		}
		else if (pos == std::string::npos || (maxSplits && num_splits == maxSplits))
		{
			if (curDoubleDelim != 0)
			{
				//Missing closer. Warn or throw exception?
			}
			// Copy the rest of the string
			ret.push_back(str.substr(start));
			break;
		}
		else
		{
			if (curDoubleDelim != 0)
			{
				curDoubleDelim = 0;
			}

			// Copy up to delimiter
			ret.push_back(str.substr(start, pos - start));
			start = pos + 1;
		}
		if (curDoubleDelim == 0)
		{
			// parse up to next real data
			start = str.find_first_not_of(singleDelims, start);
		}

		++num_splits;

	} while (pos != std::string::npos);

	return ret;
}
//-----------------------------------------------------------------------
void string_util::toLowerCase(std::string& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		tolower);
}

//-----------------------------------------------------------------------
void string_util::toUpperCase(std::string& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		toupper);
}


void string_util::toLowerCase(std::wstring& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		tolower);
}

//-----------------------------------------------------------------------
void string_util::toUpperCase(std::wstring& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		toupper);
}
//-----------------------------------------------------------------------
bool string_util::startsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string startOfThis = str.substr(0, patternLen);
	if (lowerCase)
		string_util::toLowerCase(startOfThis);

	return (startOfThis == pattern);
}
//-----------------------------------------------------------------------
bool string_util::endsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
	if (lowerCase)
		string_util::toLowerCase(endOfThis);

	return (endOfThis == pattern);
}
//-----------------------------------------------------------------------
std::string string_util::standardisePath(const std::string& init)
{
	std::string path = init;

	std::replace(path.begin(), path.end(), '\\', '/');
	if (path[path.length() - 1] != '/')
		path += '/';

	return path;
}
//-----------------------------------------------------------------------
void string_util::splitFilename(const std::string& qualifiedName, std::string& outBasename, std::string& outPath)
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

void string_util::splitFilename(const std::wstring& qualifiedName, std::wstring& outBasename, std::wstring& outPath)
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

void string_util::splitBaseFilename(const std::string& fullName, std::string& outBasename, std::string& outExtention)
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

void string_util::splitFullFilename(const std::string& qualifiedName, std::string& outBasename, std::string& outExtention, std::string& outPath)
{
	std::string fullName;
	splitFilename(qualifiedName, fullName, outPath);
	splitBaseFilename(fullName, outBasename, outExtention);
}

bool string_util::match(const std::string& str, const std::string& pattern, bool caseSensitive)
{
	std::string tmpStr = str;
	std::string tmpPattern = pattern;

	if (!caseSensitive)
	{
		string_util::toLowerCase(tmpStr);
		string_util::toLowerCase(tmpPattern);
	}

	std::string::const_iterator strIt = tmpStr.begin();
	std::string::const_iterator patIt = tmpPattern.begin();
	std::string::const_iterator lastWildCardIt = tmpPattern.end();
	while (strIt != tmpStr.end() && patIt != tmpPattern.end())
	{
		if (*patIt == '*')
		{
			lastWildCardIt = patIt;
			// Skip over looking for next character
			++patIt;
			if (patIt == tmpPattern.end())
			{
				// Skip right to the end since * matches the entire rest of the string
				strIt = tmpStr.end();
			}
			else
			{
				// scan until we find next pattern character
				while (strIt != tmpStr.end() && *strIt != *patIt)
					++strIt;
			}
		}
		else
		{
			if (*patIt != *strIt)
			{
				if (lastWildCardIt != tmpPattern.end())
				{
					// The last wildcard can match this incorrect sequence
					// rewind pattern to wildcard and keep searching
					patIt = lastWildCardIt;
					lastWildCardIt = tmpPattern.end();
				}
				else
				{
					// no wildwards left
					return false;
				}
			}
			else
			{
				++patIt;
				++strIt;
			}
		}

	}
	// If we reached the end of both the pattern and the string, we succeeded
	if (patIt == tmpPattern.end() && strIt == tmpStr.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

const std::string string_util::replaceAll(const std::string& source, const std::string& replaceWhat, const std::string& replaceWithWhat)
{
	std::string result = source;
	std::string::size_type pos = 0;
	while (1)
	{
		pos = result.find(replaceWhat, pos);
		if (pos == std::string::npos) break;
		result.replace(pos, replaceWhat.size(), replaceWithWhat);
		pos += replaceWithWhat.size();
	}
	return result;
}

bool string_util::StringReplace(std::wstring& path, const std::wstring& src, const std::wstring& tar)
{
	auto pos = path.find(src);
	if (pos == std::string::npos)
	{
		return false;
	}

	path.replace(pos, path.length(), tar);

	return true;
}

std::string& string_util::format(std::string& dest, const char* fmt, ...)
{
	char text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(text, sizeof(text) / sizeof(char), _TRUNCATE, fmt, args);
	va_end(args);

	dest = text;

	return dest;
}

std::wstring& string_util::format(std::wstring& dest, const wchar_t* fmt, ...)
{
	wchar_t text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnwprintf_s(text, sizeof(text) / sizeof(wchar_t), _TRUNCATE, fmt, args);
	va_end(args);

	dest = text;

	return dest;
}

const std::string string_util::format(const char* fmt, ...)
{
	char text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(text, sizeof(text) / sizeof(char), _TRUNCATE, fmt, args);
	va_end(args);

	std::string strOut(text);

	return strOut;
}

const std::wstring string_util::format(const wchar_t* fmt, ...)
{
	wchar_t text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnwprintf_s(text, sizeof(text) / sizeof(wchar_t), _TRUNCATE, fmt, args);
	va_end(args);

	std::wstring strOut(text);

	return strOut;
}

} // util
} // lax
