#pragma once

#include <string>
#include <vector>
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

namespace lax
{
namespace util
{

/**
 * @class string_util
 *
 * 문자열 관련. wide 함수들만 일단 있음. 필요하면 다른 함수들 추가
 */

#define __MB2WC(x)	L ## x
#define _MB2WC(x)	__MB2WC(x)

class string_util
{
public:
	using string_vector = std::vector<std::string>;

	static bool convert(const std::string& src, std::wstring& out, Int32 codePage = CP_ACP);
	static bool convert(const std::string& src, std::string& out, Int32 codePage = CP_ACP);
	static bool convert(const std::wstring& src, std::string& out, Int32 codePage = CP_ACP);
	static bool convert(const std::wstring& src, std::wstring& out, Int32 codePage = CP_ACP);
	static bool convert(const wchar_t* src, std::wstring& out, Int32 codePage = CP_ACP);
	static bool convert(const char* src, std::wstring& out, Int32 codePage = CP_ACP);

	//
	static void trim(std::string& str, bool left = true, bool right = true);
	static StringVector split(const std::string& str, const std::string& delims = "\t\n ", unsigned int maxSplits = 0);
	static StringVector tokenise(const std::string& str, const std::string& delims = "\t\n ", const std::string& doubleDelims = "\"", unsigned int maxSplits = 0);
	static void toLowerCase(std::string& str);
	static void toUpperCase(std::string& str);
	static void toLowerCase(std::wstring& str);
	static void toUpperCase(std::wstring& str);
	static bool startsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);
	static bool endsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);
	static std::string standardisePath(const std::string &init);
	static void splitFilename(const std::string& qualifiedName, std::string& outBasename, std::string& outPath);
	static void splitFilename(const std::wstring& qualifiedName, std::wstring& outBasename, std::wstring& outPath);
	static void splitFullFilename(const std::string& qualifiedName, std::string& outBasename, std::string& outExtention, std::string& outPath);
	static void splitBaseFilename(const std::string& fullName, std::string& outBasename, std::string& outExtention);
	static bool match(const std::string& str, const std::string& pattern, bool caseSensitive = true);
	static const std::string replaceAll(const std::string& source, const std::string& replaceWhat, const std::string& replaceWithWhat);
	static std::string& format(std::string& dest, const char* fmt, ...);
	static std::wstring& format(std::wstring& dest, const wchar_t* fmt, ...);
	static const std::string StringUtil::format(const char* fmt, ...);
	static const std::wstring StringUtil::format(const wchar_t* fmt, ...);

	/*
	 * msg 로 전달된 문자열을 delimiter 단위로 쪼개어 tokenizedWords 벡터에 삽입합니다.
	 */
	static void TokenizeStr(const std::wstring& msg, std::vector< std::wstring >& tokenizedWords, const std::wstring& delimiter = L" ");

	static bool StringReplace(std::wstring& path, const std::wstring& src, const std::wstring& tar);

	static const std::string empty;
};

} // namespace util 
} // namespace lax

