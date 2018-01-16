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
class string_util
{
public:
	using svec = std::vector<std::string>;
	using wvec = std::vector<std::wstring>;

	/// trim whitespaces from string
	static void trim(
		std::string& str, 
		bool left = true, 
		bool right = true
	);

	static void trim(
		std::wstring& str, 
		bool left = true, 
		bool right = true
	);

	/// windows code conversion from narrow to wide with a code page
	static bool convert(
		const std::string& src, 
		std::wstring& out, 
		uint32_t code_page = CP_ACP
	);

	static bool convert(
		const std::wstring& src, 
		std::string& out, 
		uint32_t code_page = CP_ACP
	);

	static bool convert(
		const std::string& src, 
		std::string& out, 
		uint32_t code_page = CP_ACP
	);

	static bool convert(
		const std::wstring& src, 
		std::wstring& out, 
		uint32_t code_page = CP_ACP
	);

	static bool convert(
		const wchar_t* src, 
		std::wstring& out, 
		uint32_t code_page = CP_ACP
	);

	static bool convert(
		const char* src, 
		std::wstring& out, 
		uint32_t code_page = CP_ACP
	);

	/// split a string
	static svec tokenize(
		const std::string& str, 
		const std::string& delims = "\t\n ", 
		unsigned int max_splits = 0
	);

	/// split a string further spliting with second delimiter. 
	/** 
	 * example: 
	 *   a,c:d,e:f
	 *   first tokenize with ,, then each one is tokenized with :
	 */
	static svec tokenize_nested(
		const std::string& str, 
		const std::string& delims = "\t\n ", 
		const std::string& second_delim = "\"", 
		unsigned int max_splits = 0
	);

	/// msg 로 전달된 문자열을 delimiter 단위로 쪼개어 tokens 벡터에 삽입합니다.
	static void tokenize(
		const std::wstring& msg, 
		wvec& tokens, 
		const std::wstring& delimiter = L" "
	);

	/// cast conversion
	static void to_lowercase(
		std::string& str
	);

	static void to_uppercase(
		std::string& str
	);

	static void to_lowercase(
		std::wstring& str
	);

	static void to_uppercase(
		std::wstring& str
	);

	static bool starts_with(
		const std::string& str, 
		const std::string& pattern, 
		bool lowerCase = true
	);

	static bool ends_with(
		const std::string& str, 
		const std::string& pattern, 
		bool lowerCase = true
	);

	static bool match(
		const std::string& str, 
		const std::string& pattern, 
		bool caseSensitive = true
	);

	static bool replace(
		std::wstring& path, 
		const std::wstring& src, 
		const std::wstring& tar
	);

	static const std::string replace_all(
		const std::string& source, 
		const std::string& target, 
		const std::string& replace
	);

	/// format. max character is 1024. return value refers to the dest.
	static std::string& format( 
		std::string& dest, 
		const char* fmt, 
		...  
	);
	
	/// format. max character is 1024. return value refers to the dest.
	static std::wstring& format(
		std::wstring& dest, 
		const wchar_t* fmt, 
		...
	);

	/// format. max character is 1024
	static const std::string format(
		const char* fmt, 
		...
	);

	/// format. max character is 1024
	static const std::wstring format(
		const wchar_t* fmt, 
		...
	);

	static std::string standardize_path(
		const std::string& init
	);

	static void split_path(
		const std::string& qualifiedName, 
		std::string& outBasename, 
		std::string& outPath
	);

	static void split_path(
		const std::wstring& qualifiedName, 
		std::wstring& outBasename, 
		std::wstring& outPath
	);

	static void split_filename(
		const std::string& fullName, 
		std::string& outBasename, 
		std::string& outExtention
	);

	static const std::string empty;
};

} // namespace util 
} // namespace lax

