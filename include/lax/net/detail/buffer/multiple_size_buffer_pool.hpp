#pragma once 

#include <lax/net/detail/buffer/fixed_size_buffer_pool.hpp>

#include <atomic>
#include <mutex>
#include <memory>
#include <vector>

namespace lax
{
namespace net
{

/// multiple size buffer pool
/** 
 * features: 
 *  - thread safe 
 *  - blocks (c-style)
 *  - 
 */
class multiple_size_buffer_pool
{
public:
	using buffer = fixed_size_buffer_pool::buffer;

	struct config
	{
		std::size_t start_power_of_2 = 6;	// 64 bytes 
		std::size_t	steps = 16;				// 2^24 = 1MB   
	};

	struct stat
	{
		std::atomic<uint32_t>	os_alloc_count = 0;
		std::atomic<uint32_t>	os_alloc_bytes = 0;
		std::atomic<uint32_t>	total_os_alloc_bytes = 0;
		std::atomic<uint32_t>	total_os_release_bytes = 0;
	};

public:
	multiple_size_buffer_pool()
		: config_()
	{
		expect(config_.steps > 0);
		expect(config_.steps < 32);
		expect(config_.start_power_of_2 > 0);

		init_pools();
	}

	~multiple_size_buffer_pool()
	{
	}

	buffer::ptr alloc(std::size_t required_size)
	{
		for (int i = 0; i < pools_.size(); ++i)
		{
			if (pools_[i]->get_length() >= required_size)
			{
				return pools_[i]->alloc();
			}
		}

		// allocate from OS
		++stat_.os_alloc_count;
		stat_.os_alloc_bytes += static_cast<uint32_t>(required_size);
		stat_.total_os_alloc_bytes += stat_.os_alloc_bytes;

		return std::make_shared<buffer>(new uint8_t[required_size], required_size);
	}

	void release(buffer::ptr block)
	{
		if ( block->is_allocated_from_os() )
		{ 
			check(stat_.os_alloc_count > 0);
			check(stat_.os_alloc_bytes > 0);
			check(block->get_pool() == nullptr);

			--stat_.os_alloc_count;
			stat_.os_alloc_bytes -= static_cast<uint32_t>(block->capacity());
			stat_.total_os_release_bytes += static_cast<uint32_t>(block->capacity());

			block.reset();
		}
		else
		{
			block->get_pool()->release(block);
		}
	}

	const fixed_size_buffer_pool* get_pool(std::size_t size) const
	{
		for (int i = 0; i < pools_.size(); ++i)
		{
			if (pools_[i]->get_length() == size)
			{
				return pools_[i].get();
			}
		}

		return nullptr;
	}

	const stat& get_stat() const
	{
		return stat_;
	}

	void dump_stat() const;

private: 
	void init_pools()
	{
		auto size = 1 << config_.start_power_of_2;

		for (std::size_t step = 0; step < config_.steps; ++step)
		{
			pools_.push_back(std::make_unique<fixed_size_buffer_pool>(size));

			size <<= 1;
		}
	}

private: 
	using pools = std::vector<std::unique_ptr<fixed_size_buffer_pool>>;

	config	config_;
	pools	pools_;
	stat	stat_;
};

} // net 
} // lax

