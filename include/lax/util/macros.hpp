#pragma once 

#include <lax/util/config.hpp>

#include <cassert>

namespace lax
{
void check_(bool cond, const char* msg, const char* func, const char* file, int line);
}

#ifdef _DEBUG
	#define check(c) (void)(!!(c) || (lax::check_(!!(c), #c, __FUNCTION__, __FILE__, __LINE__), 0)); assert(c);				
#else 
	#if LAX_ENABLE_RELEASE_CHECK  == 0
		#define check(c) 
	#else 
		#define check(c) (void)(!!(c) || (lax::check_(!!(c), #c, __FUNCTION__, __FILE__, __LINE__), 0))
	#endif
#endif 

/// precondition
#define expect(c) check(c)		 
/// post condition
#define ensure(c) check(c)

#define return_if(c, v) if ((c)) return v
#define break_if(c, v) if ((c)) break 
#define continue_if(c, v) if ((c)) continue 


