#pragma once

#include <spdlog/spdlog.h>

namespace lax
{
namespace util
{

/// 
class log
{
public: 
	static unsigned int async_queue_size;		// default 1024*1024. must be power of 2 
	static unsigned int flush_interval_seconds; // default 2 seconds
	static const char* file_prefix;				// default log_ (ex. logs/log_)
	static const char* log_pattern;				// default [%H:%M:%S %z][%t]  
	static bool enable_console;					// default true
	static bool retry_on_overflow;				// default true

public: 
	/// initialize only once;
	static void init();

	static std::shared_ptr<spdlog::logger> get()
	{
		init(); // make it sure 
		return spdlog::get(name);
	}

private: 
	static void create_log_folder(const char* path);

private:
	static const char* name;					// default "system"

private:
	static std::atomic<bool> initialized_;
};

inline std::shared_ptr<spdlog::logger> log()
{
	return log::get();
}

} // util
} // lax
