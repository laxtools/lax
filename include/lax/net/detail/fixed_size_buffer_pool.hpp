#pragma once 

#include <lax/net/detail/segment.hpp>
#include <lax/util/concurrent_queue.hpp>

#include <atomic>
#include <map>
#include <memory>
#include <vector>

namespace lax
{
namespace net
{

class fixed_size_buffer_pool
{
public: 
	class buffer
	{
	public:
		using ptr = std::shared_ptr<buffer>;

		buffer(uint8_t* data, std::size_t len)
			: data_(data)
			, size_(len)
		{
			expect(data_);
			expect(size_ > 0);
		}

		uint8_t* data() 
		{
			return data_;
		}

		const uint8_t* data() const
		{
			return data_;
		}

		std::size_t capacity() const
		{
			return size_;
		}

	private:
		uint8_t*		data_ = nullptr;
		std::size_t		size_ = 0;
	};

	struct stat
	{
		std::atomic<uint32_t> alloc_count = 0;
		std::atomic<uint32_t> total_alloc_count = 0;
		std::atomic<uint32_t> total_release_count = 0;
	};

public:
	explicit fixed_size_buffer_pool(std::size_t length)
		: length_(length)
	{
	}

	~fixed_size_buffer_pool()
	{
	}

	std::size_t get_length() const
	{
		return length_;
	}

	std::size_t get_size() const
	{
		return blocks_.unsafe_size();
	}

	buffer::ptr alloc()
	{
		buffer::ptr block;

		++stat_.alloc_count;
		++stat_.total_alloc_count;

		if (blocks_.pop(block))
		{
			return block;
		}
			
		return std::make_shared<buffer>( new uint8_t[get_length()], get_length() );
	}

	void release(const buffer::ptr block)
	{
		expect(block->data());
		expect(block->capacity() == length_);
		return_if(!block->data());
		return_if(block->capacity() != length_);

		--stat_.alloc_count;
		++stat_.total_release_count;

		blocks_.push(block);
	}

	const stat& get_stat() const
	{
		return stat_;
	}

private: 
	util::concurrent_queue<buffer::ptr> blocks_;
	std::size_t length_ = 0;
	stat stat_;
};

} // net 
} // lax

