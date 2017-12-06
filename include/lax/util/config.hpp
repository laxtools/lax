#pragma once 

#define LAX_IGNORE_MACRO_INSUFFICIENT_ARGUMENT 1

#if LAX_IGNORE_MACRO_INSUFFICIENT_ARGUMENT
	#pragma warning(disable: 4003)	// macro 인수 부족 경고. 편하게 쓰기 위해 위반.
#endif

#define LAX_ENABLE_CHECK_LOG		1	// check 위반 사항을 로그로 남김 
#define LAX_ENABLE_RELEASE_CHECK	1	// release 모드에서 check 처리 

#ifdef _MSC_VER 
#define LAX_ENABLE_CONCURRENT_QUEUE 1
#else 
#define LAX_ENABLE_CONCURRENT_QUEUE 0
#endif
