#include "stdafx.h"

#include <lax/util/logger.hpp>
#include <lax/util/macros.hpp>
#include <lax/util/platform.hpp>

#include <filesystem>

namespace lax
{
namespace util
{

const char*		log::name = "system";
bool			log::enable_console = true;
unsigned int	log::async_queue_size = 4096;
unsigned int	log::flush_interval_seconds = 2;
const char*		log::file_prefix = "logs/system";
const char*		log::log_pattern = "[%Y-%m-%d %H:%M:%S][%t][%L] %v";
bool			log::retry_on_overflow = true;


std::recursive_mutex log::mutex_;
std::atomic<bool> log::initialized_ = false;

void log::init()
{
	// 초기화 중 여러 곳에서 호출할 경우 블럭 시킴
	std::lock_guard<std::recursive_mutex> lock(mutex_);

	return_if(initialized_);

	spdlog::set_pattern(log_pattern);

	auto retry = retry_on_overflow ? 
			spdlog::async_overflow_policy::block_retry : 
			spdlog::async_overflow_policy::discard_log_msg; 

	spdlog::set_async_mode(
		async_queue_size,
		retry, 
		nullptr,
		std::chrono::seconds(2));

	create_log_folder(file_prefix);

	auto s1 = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
	auto s2 = std::make_shared<spdlog::sinks::daily_file_sink_mt>(file_prefix, 0, 0);

	(void)spdlog::create(name, { s1, s2 });

	initialized_ = true;
}

using namespace std::experimental::filesystem::v1;

void log::create_log_folder(const char* path)
{
	auto full_path = platform::get_current_dir() + "\\logs";

	create_directory(full_path);
}

} // util
} // lax
