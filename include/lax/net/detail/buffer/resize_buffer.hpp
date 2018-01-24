#pragma once 

#include <lax/net/detail/buffer/multiple_size_buffer_pool.hpp>
#include <lax/net/detail/buffer/resize_buffer_iterator.hpp>
#include <lax/util/macros.hpp>

#include <algorithm>
#include <vector>

namespace lax
{
namespace net
{

/// increasing sized buffer
/** 
 * not thread-safe. 
 * - needs to use a lock if used from multiple threads 
 * - if 1-recv is used, then lock is not required
 */
class resize_buffer 
{
public: 
	using pool = multiple_size_buffer_pool;

public: 
	using value_type = uint8_t;
	using iterator = resize_buffer_iterator<value_type>;
	using const_iterator = resize_buffer_iterator<const value_type>;

	resize_buffer(std::size_t initial_size = 8*1024)
		: initial_size_(initial_size)
	{
		expect(initial_size_ > 0);
		expect(pos_ == 0);
		expect(!buf_);
	}

	~resize_buffer()
	{
		if (buf_)
		{
			pool_.release(buf_);
			buf_.reset();
		}

		ensure(!buf_);
	}

	/// len만큼 쓴다. seg 부족하면 풀에서 할당
	std::size_t append(const uint8_t* p, std::size_t len)
	{
		expect(p != nullptr);
		expect(len > 0);

		reserve(len);

		check(buf_);
		check(buf_->capacity() >= pos_ + len);

		::memcpy_s(buf_->data() + pos_, buf_->capacity() - pos_, p, len);

		pos_ += len;

		ensure(pos_ <= buf_->capacity());

		return len;
	}
	
	/// 편의 함수.
	std::size_t append(const char* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	/// 편의 함수
	std::size_t append(const void* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	/// 전체 바이트 크기 돌려줌
	std::size_t size() const
	{
		return pos_;
	}

	std::size_t capacity() const
	{
		return_if(!buf_, 0);
		return buf_->capacity();
	}

	/// change the size of buffer. position is changed.
	void resize(const std::size_t new_size)
	{
		if (new_size <= capacity())
		{
			pos_ = new_size; // change size(). STL
			return;
		}

		reserve(new_size - capacity());

		ensure(buf_);

		pos_ = new_size;  

		// STL resize changes : 
		// - size(). therfore pos_ 
		// - elements from prev size up to new size
	}

	const uint8_t* data() const
	{
		return_if(!buf_, nullptr);
		return buf_->data();
	}

	/// use this with care
	uint8_t* data() 
	{
		return_if(!buf_, nullptr);
		return buf_->data();
	}


	const uint8_t& at(std::size_t pos) const
	{
		check(buf_);
		return_if(!buf_, 0); 

		check(pos < size());
		return_if(pos >= size(), 0);
		
		return buf_->data()[pos];
	}

	/// 세그먼트 앞으로 돌린다.
	void rewind(std::size_t pos = 0)
	{
		expect(pos < size());
		return_if(pos >= size()); // safe. no effect

		pos_ = pos;
	}

	/// block에서 count만큼 제거 
	/** 
	 * memmove를 사용하고 느릴 수 있으므로 
	 * 호출이 많지 않도록 사용해야 함
	 */
	void pop_front(std::size_t count)
	{
		check(buf_);
		return_if(!buf_);

		check(count > 0);
		return_if(count == 0);

		count = std::min(count, size());

		::memmove_s(
			buf_->data(), buf_->capacity(), 
			buf_->data() + count, pos_ - count
		);

		pos_ -= count;
	}

	iterator begin() 
	{ 
		return_if(!buf_, 0);
		return iterator(buf_->data()); 
	}

	iterator end() 
	{ 
		return_if(!buf_, 0);
		return iterator(&buf_->data()[pos_]); 
	}

	const_iterator cbegin() 
	{ 
		return_if(!buf_, 0);
		return const_iterator(buf_->data()); 
	}

	const_iterator cend() 
	{ 
		return_if(!buf_, 0);
		return const_iterator(&buf_->data()[pos_]); 
	}

	/// for test and debugging
	const fixed_size_buffer_pool* get_pool(std::size_t size) const
	{
		return pool_.get_pool(size);
	}

private: 
	void reserve(std::size_t len)
	{
		expect(len > 0); 
		if (!buf_)
		{
			check(pos_ == 0);
			buf_ = pool_.alloc(std::max<std::size_t>(len, initial_size_));
		}
		else
		{
			if (buf_->capacity() < (pos_ + len))
			{
				auto nbuf = pool_.alloc(pos_ + len);

				::memcpy_s(nbuf->data(), nbuf->capacity(), buf_->data(), pos_);

				pool_.release(buf_);

				buf_ = nbuf;
			}
		}

		ensure(buf_);
		ensure(capacity() >= len);
	}

private: 
	static pool				pool_;
	std::size_t				initial_size_ = 1024;
	pool::buffer::ptr		buf_;
	std::size_t				pos_ = 0;
};

} // net 
} // lax

