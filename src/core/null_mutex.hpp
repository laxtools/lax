#pragma once 

namespace lax
{
namespace util
{
struct null_mutex
{
	null_mutex() = default;

	void lock() {}
	void unlock() noexcept {}
	bool try_lock() { return true; }
};

} // util
} // lax
