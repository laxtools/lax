#pragma once 

#include <lax/util/config.hpp>

#if LAX_ENABLE_CONCURRENT_QUEUE 
	#include <concurrent_queue.h>
#else 
	#include <deque>
	#include <mutex>
#endif

namespace lax
{
namespace util
{

template <typename T>
class concurrent_queue
{
public:
	concurrent_queue();
	~concurrent_queue();

	void push(T v);

	bool pop(T& v);

	/// get size. note: thread unsafe. 
	std::size_t unsafe_size() const;

private: 
#if LAX_ENABLE_CONCURRENT_QUEUE  
	concurrency::concurrent_queue<T> q_;
#else
	std::deque<T> q_; 
	std::recursive_mutex mutex_;
#endif
};

} // util
} // lax

#include <lax/util/concurrent_queue.inl>
