#include <lax/util/concurrent_queue.hpp>

namespace lax
{
namespace util
{

template <typename T>
concurrent_queue<T>::concurrent_queue()
{
}

template <typename T>
concurrent_queue<T>::~concurrent_queue()
{
}

#if LAX_ENABLE_CONCURRENT_QUEUE  

template <typename T>
void concurrent_queue<T>::push(T v)
{
	q_.push(v);
}

template <typename T>
bool concurrent_queue<T>::pop(T& v)
{
	return q_.try_pop(v);
}

template <typename T>
std::size_t concurrent_queue<T>::unsafe_size() const
{
	return q_.unsafe_size();
}

#else 
template <typename T>
void concurrent_queue<T>::push(T v)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);
	q_.push(v);
}

template <typename T>
bool concurrent_queue<T>::pop(T& v)
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	if (q_.empty())
	{
		return false;
	}

	v = q_.front(); 
	q_.pop_front();

	return true;
}

template <typename T>
std::size_t concurrent_queue<T>::unsafe_size() const
{
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	return  q_.size();
}

#endif

} // util 
} // lax
