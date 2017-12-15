#pragma once 

#include <lax/channel/channel.hpp>

namespace lax
{
namespace channel
{

class channel_map
{
public: 
	using ptr = channel::ptr;
	using key_t = channel::key_t;
	using config = channel::config;

public: 
	static channel_map& get();

	~channel_map(); 

	ptr create(const key_t& key, const config& cfg);

	ptr find(const key_t& key);

	bool destroy(const key_t& key);

private: 
	using map = std::map<key_t, ptr>;

	channel_map();

private: 
	std::shared_timed_mutex		mutex_;
	map							map_;
};

} // channel
} // lax