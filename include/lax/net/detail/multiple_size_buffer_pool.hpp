#pragma once 

#include <lax/net/detail/fixed_size_buffer_pool.hpp>

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
		std::size_t	steps = 25;				// upto 1024 MB
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

		throw std::exception("pool dosn't have the required memory size to alloc");
	}

	void release(const buffer::ptr block)
	{
		for (int i = 0; i < pools_.size(); ++i)
		{
			if (pools_[i]->get_length() == block->capacity())
			{
				return pools_[i]->release(block);
			}
		}

		throw std::exception("pool dosn't have the size to release");
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

	config config_;
	pools pools_;
};

} // net 
} // lax

