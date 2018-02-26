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
 * ���ڿ� ����. wide �Լ��鸸 �ϴ� ����. �ʿ��ϸ� �ٸ� �Լ��� �߰�
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

	/// msg �� ���޵� ���ڿ��� delimiter ������ �ɰ��� tokens ���Ϳ� �����մϴ�.
	static void tokenize(
		const std::wstring& msg, 
		wvec& tokens, 
		const std::wstring& delimiter = L" "
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

