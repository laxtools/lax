#include "Prefix.h"
#include "StringUtil.h"
#include <windows.h>



namespace { 


const std::string StringUtil::EMPTY;

void StringUtil::trim(std::string& str, bool left, bool right)
{
	static const std::string delims = " \t\r";
    
	if(right)
		str.erase(str.find_last_not_of(delims)+1); // trim right
     
	if(left)
		str.erase(0, str.find_first_not_of(delims)); // trim left
}

Bool StringUtil::Convert( const std::string& src, std::string& out, Int32 codePage )
{
	std::wstring wstr;
	Convert( src, wstr );
	Convert( wstr, out, codePage );

	return true;
}

Bool StringUtil::Convert( const std::wstring& src, std::wstring& out, Int32 codePage )
{
	codePage;
	out = src;
	return true;
}

Bool StringUtil::Convert(const wchar_t* src, std::wstring& out, Int32 codePage)
{
	codePage;
	out = src;
	return true;
}

Bool StringUtil::Convert(const char* src, std::wstring& out, Int32 codePage)
{
	out.clear();

	int slen = std::min<int>(8192, (int)strlen(src));
	if (slen == 0)
	{
		return true;
	}

	int wlen = MultiByteToWideChar(codePage, 0, src, slen, NULL, NULL);
	LPWSTR np = (LPWSTR)_alloca(sizeof(WCHAR) * (wlen + 1)); // 스택에서 할당
	::memset(np, 0, (wlen + 1) * sizeof(WCHAR));
	MultiByteToWideChar(codePage, 0, src, slen, np, wlen);

	out.append(np);

	return true;
}


Bool StringUtil::Convert( const std::string& src, std::wstring& out, Int32 codePage )
{
	out.clear();

	int slen = std::min<int>( 8192, (int)strlen( src.c_str() ) );
	if ( slen == 0 )
	{
		return true;
	}

	int wlen = MultiByteToWideChar( codePage, 0, src.c_str(), slen, NULL, NULL );
	LPWSTR np = (LPWSTR)_alloca( sizeof( WCHAR ) * ( wlen + 1 ) ); // 스택에서 할당
	::memset( np, 0, (wlen + 1 ) * sizeof( WCHAR ) );
	MultiByteToWideChar( codePage, 0, src.c_str(), slen, np, wlen );

	out.append( np );

	return true;
}

Bool StringUtil::Convert( const std::wstring& s, std::string& out, Int32 codePage )
{
	out.clear();

	int slen = std::min<int>( 8192, (int)wcslen( s.c_str() ) );

	int wlen = WideCharToMultiByte( codePage, 0, s.c_str(), slen, NULL, 0, NULL, NULL );
	if ( wlen == 0 )
	{
		return true;
	}

	LPSTR np = (LPSTR)_alloca( sizeof( CHAR ) * ( wlen + 1 ) );
	::memset( np, 0, (wlen + 1 ) * sizeof( CHAR ) );
	WideCharToMultiByte( codePage, 0, s.c_str(), slen, np, wlen, NULL, NULL );

	out.append( np );

	return true;
}


void StringUtil::TokenizeStr( const std::wstring& msg, std::vector< std::wstring >& tokenizedWords, const std::wstring& delimiter )
{
	std::wstring::size_type last_pos = msg.find_first_not_of( delimiter, 0 );
	std::wstring::size_type pos = msg.find_first_of( delimiter, last_pos );
	while( std::wstring::npos != pos || std::wstring::npos != last_pos )
	{
		std::wstring var = msg.substr( last_pos, pos - last_pos );
		tokenizedWords.push_back( var );
		last_pos += var.length() + delimiter.length();
		if( last_pos > msg.length() )
			last_pos = std::wstring::npos;
		pos = msg.find_first_of( delimiter, last_pos );
	}
}

Bool StringUtil::ParserInt( const std::wstring& msg, const std::wstring& delimiter, Int32& value )
{
	std::wstring::size_type last_pos = msg.find_first_not_of( delimiter, 0 );
	std::wstring::size_type pos = msg.find_first_of( delimiter, last_pos );

	std::wstring data = msg.substr( last_pos, pos - last_pos );
	if ( data.empty() )
	{
		return false;
	}

	value = _wtoi( data.c_str() );

	return true;
}


Bool StringUtil::ParserFloat( const std::wstring& msg, const std::wstring& delimiter, Float& value )
{
	std::wstring::size_type last_pos = msg.find_first_not_of( delimiter, 0 );
	std::wstring::size_type pos = msg.find_first_of( delimiter, last_pos );

	std::wstring data = msg.substr( last_pos, pos - last_pos );
	if ( data.empty() )
	{
		return false;
	}

	value = static_cast<Float>(_wtof( data.c_str() ) );

	return true;
}


StringUtil::StringVector StringUtil::split( const std::string& str, const std::string& delims, unsigned int maxSplits)
{
	StringVector ret;
	// Pre-allocate some space for performance
	ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

	unsigned int numSplits = 0;

	// Use STL methods 
	size_t start, pos;
	start = 0;
	do 
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
		{
			// Copy the rest of the string
			ret.push_back( str.substr(start) );
			break;
		}
		else
		{
			// Copy up to delimiter
			ret.push_back( str.substr(start, pos - start) );
			start = pos + 1;
		}
		// parse up to next real data
		start = str.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != std::string::npos);



	return ret;
}
//-----------------------------------------------------------------------
StringUtil::StringVector StringUtil::tokenise( const std::string& str, const std::string& singleDelims, const std::string& doubleDelims, unsigned int maxSplits)
{
	StringVector ret;
	// Pre-allocate some space for performance
	ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

	unsigned int numSplits = 0;
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
		else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
		{
			if (curDoubleDelim != 0)
			{
				//Missing closer. Warn or throw exception?
			}
			// Copy the rest of the string
			ret.push_back( str.substr(start) );
			break;
		}
		else
		{
			if (curDoubleDelim != 0)
			{
				curDoubleDelim = 0;
			}

			// Copy up to delimiter
			ret.push_back( str.substr(start, pos - start) );
			start = pos + 1;
		}
		if (curDoubleDelim == 0)
		{
			// parse up to next real data
			start = str.find_first_not_of(singleDelims, start);
		}

		++numSplits;

	} while (pos != std::string::npos);

	return ret;
}
//-----------------------------------------------------------------------
void StringUtil::toLowerCase(std::string& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		tolower);
}

//-----------------------------------------------------------------------
void StringUtil::toUpperCase(std::string& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		toupper);
}


void StringUtil::toLowerCase(std::wstring& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		tolower);
}

//-----------------------------------------------------------------------
void StringUtil::toUpperCase(std::wstring& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		toupper);
}
//-----------------------------------------------------------------------
Bool StringUtil::startsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string startOfThis = str.substr(0, patternLen);
	if (lowerCase)
		StringUtil::toLowerCase(startOfThis);

	return (startOfThis == pattern);
}
//-----------------------------------------------------------------------
Bool StringUtil::endsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
	if (lowerCase)
		StringUtil::toLowerCase(endOfThis);

	return (endOfThis == pattern);
}
//-----------------------------------------------------------------------
std::string StringUtil::standardisePath(const std::string& init)
{
	std::string path = init;

	std::replace( path.begin(), path.end(), '\\', '/' );
	if( path[path.length() - 1] != '/' )
		path += '/';

	return path;
}
//-----------------------------------------------------------------------
void StringUtil::splitFilename(const std::string& qualifiedName, std::string& outBasename, std::string& outPath)
{
	std::string path = qualifiedName;
	// Replace \ with / first
	std::replace( path.begin(), path.end(), '\\', '/' );
	// split based on final /
	size_t i = path.find_last_of('/');

	if (i == std::string::npos)
	{
		outPath.clear();
		outBasename = qualifiedName;
	}
	else
	{
		outBasename = path.substr(i+1, path.size() - i - 1);
		outPath = path.substr(0, i+1);
	}

}

void StringUtil::splitFilename(const std::wstring& qualifiedName, std::wstring& outBasename, std::wstring& outPath)
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

//-----------------------------------------------------------------------
void StringUtil::splitBaseFilename(const std::string& fullName, std::string& outBasename, std::string& outExtention)
{
	size_t i = fullName.find_last_of(".");
	if (i == std::string::npos)
	{
		outExtention.clear();
		outBasename = fullName;
	}
	else
	{
		outExtention = fullName.substr(i+1);
		outBasename = fullName.substr(0, i);
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------
void StringUtil::splitFullFilename(	const std::string& qualifiedName,	std::string& outBasename, std::string& outExtention, std::string& outPath )
{
	std::string fullName;
	splitFilename( qualifiedName, fullName, outPath );
	splitBaseFilename( fullName, outBasename, outExtention );
}
//-----------------------------------------------------------------------
Bool StringUtil::match(const std::string& str, const std::string& pattern, bool caseSensitive)
{
	std::string tmpStr = str;
	std::string tmpPattern = pattern;
	if (!caseSensitive)
	{
		StringUtil::toLowerCase(tmpStr);
		StringUtil::toLowerCase(tmpPattern);
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
				while(strIt != tmpStr.end() && *strIt != *patIt)
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

const std::string StringUtil::replaceAll(const std::string& source, const std::string& replaceWhat, const std::string& replaceWithWhat)
{
	std::string result = source;
	std::string::size_type pos = 0;
	while(1)
	{
		pos = result.find(replaceWhat,pos);
		if (pos == std::string::npos) break;
		result.replace(pos,replaceWhat.size(),replaceWithWhat);
		pos += replaceWithWhat.size();
	}
	return result;
}

Bool StringUtil::StringReplace( std::wstring& path, const std::wstring& src, const std::wstring& tar )
{
	auto pos = path.find(src);
	if ( pos == std::string::npos )
	{
		return false;
	}

	path.replace( pos, path.length(), tar );

	return true;
}



std::string& StringUtil::format(std::string& dest, const char* fmt, ...)
{
	char text[1024];

	va_list args;
	va_start(args, fmt);	
	_vsnprintf_s(text, sizeof(text) / sizeof(char), _TRUNCATE, fmt, args);
	va_end(args);

	dest = text;

	return dest;
}



std::wstring& StringUtil::format(std::wstring& dest, const wchar_t* fmt, ...)
{
	wchar_t text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnwprintf_s(text, sizeof(text) / sizeof(wchar_t), _TRUNCATE, fmt, args);
	va_end(args);

	dest = text;

	return dest;
}



const std::string StringUtil::format(const char* fmt, ...)
{
	char text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(text, sizeof(text) / sizeof(char), _TRUNCATE, fmt, args);
	va_end(args);

	std::string strOut = text;

	return strOut;
}



const std::wstring StringUtil::format(const wchar_t* fmt, ...)
{
	wchar_t text[1024];

	va_list args;
	va_start(args, fmt);
	_vsnwprintf_s(text, sizeof(text) / sizeof(wchar_t), _TRUNCATE, fmt, args);
	va_end(args);

	std::wstring strOut = text;

	return strOut;
}



std::string StringUtil::ToString(UInt32 value)
{
	char buff[12] = { 0, };
	_ultoa_s(value, buff, 10);
	return std::string(buff);
}

std::string StringUtil::ToString(Int32 value)
{
	char buff[12] = { 0, };
	_itoa_s(value, buff, 10);
	return std::string(buff);
}

std::string StringUtil::ToString(UShort value)
{
	return ToString((Int32)value);
}





} // namespace mu2
